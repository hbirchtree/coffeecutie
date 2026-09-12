#!/usr/bin/env node
// Collector for the GL trace stream produced by glw/trace.h.
//
// The wasm build streams packed binary records over a WebSocket rather than
// keeping them in the page, so traces survive a crash and are not bounded by
// browser storage. This decodes them, writes a readable log, and keeps any
// data blobs alongside it.
//
// Usage:
//   node gl_trace_server.mjs [--port 8099] [--out /tmp/gltrace]
//
// Then run the app with COFFEE_GL_TRACE=calls|errors|data and, if the port is
// not the default, COFFEE_GL_TRACE_URL=ws://127.0.0.1:<port>.
//
// No dependencies: the WebSocket handshake and frame decoding are done here,
// because the repo does not carry a ws module.

import { createServer } from 'node:http';
import { createHash } from 'node:crypto';
import { mkdir, writeFile, appendFile } from 'node:fs/promises';
import { openSync, writeSync, closeSync } from 'node:fs';
import { deflateSync } from 'node:zlib';
import { join } from 'node:path';
import { VIEWER_HTML } from './gl_trace_viewer.mjs';

const args = process.argv.slice(2);
const argOf = (name, fallback) => {
  const i = args.indexOf(name);
  return i >= 0 && args[i + 1] ? args[i + 1] : fallback;
};

const port = Number(argOf('--port', 8099));
const outDir = argOf('--out', '/tmp/gltrace');
const logPath = join(outDir, 'trace.log');
const GUID = '258EAFA5-E914-47DA-95CA-C5AB0DC85B11';

const KIND_NAME = 1;
const KIND_CALL = 2;
const KIND_FRAME = 3;
const KIND_TEX = 4;
const KIND_PUSH = 5;
const KIND_POP = 6;
const KIND_LABEL = 7;
const KIND_MSG = 8;
const KIND_XLAT = 9;
const KIND_SHOT = 10;
const WHOLE_FRAME = 0xffffffff;

// One self-contained file rather than a directory of thousands: blobs, images
// and per-frame event lists are appended as they arrive, and an index of their
// offsets is written to the footer at the end. The viewer reads the footer
// first and then slices out only the frame being looked at, so scrubbing never
// parses the whole trace.
const containerPath = join(outDir, 'trace.gltrace');
let container = null;
let containerAt = 0;
const index = { version: 1, labels: {}, frames: [], shaders: [] };
let frameEvents = [];
let frameShot = null;
let frameDraws = [];
let pendingDraw = null;

// Written synchronously rather than through a stream: a long capture buffers
// tens of megabytes, and a stream still holding them when the process is killed
// loses the footer and with it the whole index.
function stash(buf) {
  const at = containerAt;
  writeSync(container, buf);
  containerAt += buf.length;
  return { off: at, len: buf.length };
}

// WebGL has no KHR_debug, so gleam::debug hands its scope names and object
// labels straight to the trace instead. Groups indent the log; labels name the
// handles that would otherwise print as bare integers.
let depth = 0;
const labels = new Map(); // `${identifier}:${handle}` -> name

// GL object identifiers as KHR_debug numbers them, for labelled handles.
const OBJECT_KINDS = {
  0x1702: 'texture', 0x82E0: 'buffer', 0x82E1: 'shader', 0x82E2: 'program',
  0x82E3: 'query', 0x8074: 'vertex_array', 0x8D40: 'framebuffer',
  0x8D41: 'renderbuffer', 0x82E6: 'sampler', 0x82E8: 'transform_feedback',
};

// Calls whose handle argument can be resolved against a recorded label.
const BOUND_OBJECT = {
  glBindTexture: { kind: 0x1702, arg: 1 },
  glBindBuffer: { kind: 0x82E0, arg: 1 },
  glBindBufferRange: { kind: 0x82E0, arg: 2 },
  glUseProgram: { kind: 0x82E2, arg: 0 },
  glBindVertexArray: { kind: 0x8074, arg: 0 },
  glBindFramebuffer: { kind: 0x8D40, arg: 1 },
  glBindRenderbuffer: { kind: 0x8D41, arg: 1 },
  glBindSampler: { kind: 0x82E6, arg: 1 },
};

const ARG_TYPES = ['i64', 'u64', 'f64', 'ptr', 'enum', 'vec2', 'opaque', 'string'];
const ARG_STRING = 7;

// Errors worth naming; anything else is printed raw.
const GL_ERRORS = {
  0x0500: 'INVALID_ENUM',
  0x0501: 'INVALID_VALUE',
  0x0502: 'INVALID_OPERATION',
  0x0503: 'STACK_OVERFLOW',
  0x0504: 'STACK_UNDERFLOW',
  0x0505: 'OUT_OF_MEMORY',
  0x0506: 'INVALID_FRAMEBUFFER_OPERATION',
};


function crc32(buf) {
  let c, crc = 0xffffffff;
  for (let n = 0; n < buf.length; n++) {
    c = (crc ^ buf[n]) & 0xff;
    for (let k = 0; k < 8; k++) c = c & 1 ? 0xedb88320 ^ (c >>> 1) : c >>> 1;
    crc = c ^ (crc >>> 8);
  }
  return (crc ^ 0xffffffff) >>> 0;
}

function chunk(type, data) {
  const out = Buffer.alloc(8 + data.length + 4);
  out.writeUInt32BE(data.length, 0);
  out.write(type, 4, 'ascii');
  data.copy(out, 8);
  out.writeUInt32BE(crc32(out.subarray(4, 8 + data.length)), 8 + data.length);
  return out;
}

// RGBA8 rows with a zero filter byte each, deflated in stored mode.
function encodePng(pixels, w, h) {
  const stride = w * 4;
  const raw = Buffer.alloc((stride + 1) * h);
  for (let y = 0; y < h; y++) {
    raw[y * (stride + 1)] = 0;
    pixels.copy(raw, y * (stride + 1) + 1, y * stride, y * stride + stride);
  }
  // Real deflate, not stored blocks: a timeline holds hundreds of these and
  // screenshots compress by an order of magnitude.
  const zlib = deflateSync(raw, { level: 6 });

  const ihdr = Buffer.alloc(13);
  ihdr.writeUInt32BE(w, 0);
  ihdr.writeUInt32BE(h, 4);
  ihdr[8] = 8;   // bit depth
  ihdr[9] = 6;   // RGBA
  return Buffer.concat([
    Buffer.from([0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a]),
    chunk('IHDR', ihdr),
    chunk('IDAT', zlib),
    chunk('IEND', Buffer.alloc(0)),
  ]);
}

const names = new Map();
const stats = { calls: 0, frames: 0, errors: 0, bytes: 0, blobs: 0, textures: 0, dataBytes: 0,
                labels: 0, messages: 0, shaders: 0, shots: 0 };
let pending = Buffer.alloc(0);
let lines = [];
let blobIndex = 0;

function formatArg(type, value) {
  const name = ARG_TYPES[type] ?? 'u64';
  if (name === 'f64') {
    const b = Buffer.alloc(8);
    b.writeBigUInt64LE(value);
    return String(b.readDoubleLE(0));
  }
  if (name === 'enum') return '0x' + value.toString(16).toUpperCase();
  if (name === 'ptr') return value === 0n ? 'null' : '0x' + value.toString(16);
  if (name === 'vec2') {
    const lo = Number(value & 0xffffffffn) | 0;
    const hi = Number(value >> 32n) | 0;
    return `${lo}x${hi}`;
  }
  if (name === 'i64') return BigInt.asIntN(64, value).toString();
  return value.toString();
}

// Returns true when a whole record was consumed.
function decodeOne(buf, offset) {
  if (offset >= buf.length) return null;
  const kind = buf.readUInt8(offset);
  let p = offset + 1;

  if (kind === KIND_PUSH) {
    if (p + 2 > buf.length) return null;
    const len = buf.readUInt16LE(p);
    p += 2;
    if (p + len > buf.length) return null;
    const name = buf.toString('utf8', p, p + len);
    p += len;
    lines.push(`${'  '.repeat(depth)}> ${name}`);
    frameEvents.push({ t: 'push', name });
    depth++;
    return p;
  }

  if (kind === KIND_POP) {
    if (depth > 0) depth--;
    frameEvents.push({ t: 'pop' });
    return p;
  }

  if (kind === KIND_LABEL) {
    if (p + 10 > buf.length) return null;
    const identifier = buf.readUInt32LE(p);
    const handle = buf.readUInt32LE(p + 4);
    const len = buf.readUInt16LE(p + 8);
    p += 10;
    if (p + len > buf.length) return null;
    const name = buf.toString('utf8', p, p + len);
    p += len;
    labels.set(`${identifier}:${handle}`, name);
    index.labels[`${identifier}:${handle}`] = name;
    frameEvents.push({ t: 'label', kind: identifier, handle, name });
    const kindName = OBJECT_KINDS[identifier] ?? `object 0x${identifier.toString(16)}`;
    lines.push(`${'  '.repeat(depth)}[label ${kindName} ${handle} = "${name}"]`);
    stats.labels++;
    return p;
  }

  if (kind === KIND_MSG) {
    if (p + 6 > buf.length) return null;
    const severity = buf.readUInt32LE(p);
    const len = buf.readUInt16LE(p + 4);
    p += 6;
    if (p + len > buf.length) return null;
    const text = buf.toString('utf8', p, p + len);
    p += len;
    lines.push(`${'  '.repeat(depth)}[message 0x${severity.toString(16)}] ${text}`);
    frameEvents.push({ t: 'msg', severity, text });
    stats.messages++;
    return p;
  }

  if (kind === KIND_SHOT) {
    if (p + 20 > buf.length) return null;
    const frame = buf.readUInt32LE(p);
    const draw = buf.readUInt32LE(p + 4);
    const w = buf.readUInt32LE(p + 8);
    const h = buf.readUInt32LE(p + 12);
    const size = buf.readUInt32LE(p + 16);
    p += 20;
    if (p + size > buf.length) return null;
    const png = encodePng(buf.subarray(p, p + size), w, h);
    p += size;
    const shot = { ...stash(png), w, h };
    if (draw === WHOLE_FRAME) {
      frameShot = shot;
    } else {
      frameDraws.push({ n: draw, fn: pendingDraw, shot });
      pendingDraw = null;
    }
    stats.shots++;
    lines.push(
      `${'  '.repeat(depth)}[shot frame ${frame}` +
      `${draw === WHOLE_FRAME ? '' : ` draw ${draw}`} ${w}x${h}]`);
    return p;
  }

  if (kind === KIND_XLAT) {
    if (p + 8 > buf.length) return null;
    const program = buf.readUInt32LE(p);
    const len = buf.readUInt32LE(p + 4);
    p += 8;
    if (p + len > buf.length) return null;
    // ANGLE's rewritten GLSL, which is what the driver actually compiled.
    const file = `program_${program}_translated.glsl`;
    writeFile(join(outDir, file), buf.subarray(p, p + len)).catch(() => {});
    p += len;
    index.shaders.push({ program, ...stash(buf.subarray(p - len, p)) });
    stats.shaders++;
    lines.push(`${'  '.repeat(depth)}[translated program ${program} -> ${file}, ${len}B]`);
    return p;
  }

  if (kind === KIND_FRAME) {
    if (p + 4 > buf.length) return null;
    const frameIndex = buf.readUInt32LE(p);
    p += 4;
    stats.frames++;
    // Depth is not reset: a scope can legitimately straddle the swap.
    lines.push(`--- frame ${frameIndex} ---`);
    flushFrame(frameIndex);
    return p;
  }

  if (kind === KIND_NAME) {
    if (p + 4 > buf.length) return null;
    const id = buf.readUInt16LE(p);
    const len = buf.readUInt16LE(p + 2);
    p += 4;
    if (p + len > buf.length) return null;
    names.set(id, buf.subarray(p, p + len).toString('utf8'));
    return p + len;
  }

  if (kind === KIND_TEX) {
    if (p + 16 > buf.length) return null;
    const id = buf.readUInt32LE(p);
    const w = buf.readUInt32LE(p + 4);
    const h = buf.readUInt32LE(p + 8);
    const size = buf.readUInt32LE(p + 12);
    p += 16;
    if (p + size > buf.length) return null;
    const pixels = buf.subarray(p, p + size);
    p += size;
    const file = `tex_${id}_${w}x${h}.png`;
    // Written as PNG so it can just be opened; the payload is RGBA8 rows.
    writeFile(join(outDir, file), encodePng(pixels, w, h)).catch(() => {});
    stats.textures++;
    let nonzero = 0;
    for (let i = 3; i < pixels.length; i += 4) if (pixels[i - 3] | pixels[i - 2] | pixels[i - 1]) nonzero++;
    lines.push(`    [texture ${id} ${w}x${h} -> ${file}, ${nonzero} non-black pixels]`);
    return p;
  }

  if (kind === KIND_CALL) {
    if (p + 3 > buf.length) return null;
    const id = buf.readUInt16LE(p);
    const argc = buf.readUInt8(p + 2);
    p += 3;
    if (p + argc * 9 > buf.length) return null;
    const parts = [];
    const raw = [];
    const stringArgs = [];
    for (let i = 0; i < argc; i++) {
      const type = buf.readUInt8(p);
      const value = buf.readBigUInt64LE(p + 1);
      p += 9;
      parts.push(formatArg(type, value));
      raw.push(value);
      if (type === ARG_STRING) stringArgs.push(parts.length - 1);
    }
    // Name bytes follow the fixed-width arguments, in argument order.
    for (const idx of stringArgs) {
      if (p + 2 > buf.length) return null;
      const len = buf.readUInt16LE(p);
      p += 2;
      if (p + len > buf.length) return null;
      parts[idx] = JSON.stringify(buf.toString('utf8', p, p + len));
      p += len;
    }
    if (p + 8 > buf.length) return null;
    const error = buf.readUInt32LE(p);
    // Bytes the call moves. A buffer allocation names a size but passes no
    // pointer, so this is recorded even when nothing was captured.
    const declared = buf.readUInt32LE(p + 4);
    p += 8;

    let dataSize = 0;
    if (declared > 0) {
      if (p + 4 > buf.length) return null;
      dataSize = buf.readUInt32LE(p);
      p += 4;
      if (p + dataSize > buf.length) return null;
    }

    let blobNote = '';
    let blobRef = null;
    if (dataSize > 0) {
      const blob = buf.subarray(p, p + dataSize);
      blobRef = stash(Buffer.from(blob));
      blobNote = dataSize < declared
        ? ` data=${declared}B (${dataSize}B captured)`
        : ` data=${dataSize}B`;
      stats.blobs++;
      stats.dataBytes += declared;
    } else if (declared > 0) {
      blobNote = ` data=${declared}B`;
      stats.dataBytes += declared;
    }
    p += dataSize;

    stats.calls++;
    let errNote = '';
    if (error !== 0) {
      stats.errors++;
      errNote = ` !! ${GL_ERRORS[error] ?? '0x' + error.toString(16)}`;
    }
    const fn = names.get(id) ?? `fn${id}`;
    // A labelled handle is far more use than its number, so name it inline.
    let labelNote = '';
    const bound = BOUND_OBJECT[fn];
    if (bound && raw.length > bound.arg) {
      const name = labels.get(`${bound.kind}:${Number(raw[bound.arg])}`);
      if (name) labelNote = ` // ${name}`;
    }
    lines.push(
      `${'  '.repeat(depth)}${fn}(${parts.join(', ')})${blobNote}${errNote}${labelNote}`);
    const ev = { t: 'call', fn, args: parts };
    if (error !== 0) ev.err = error;
    if (declared > 0) ev.size = declared;
    if (blobRef) ev.blob = blobRef;
    if (labelNote) ev.label = labelNote.slice(4);
    frameEvents.push(ev);
    // Remembered so the shot that follows can say which draw it belongs to.
    if (fn.startsWith('glDraw')) pendingDraw = fn;
    return p;
  }

  // Unknown kind: the stream is out of sync and cannot be recovered.
  throw new Error(`unknown record kind ${kind} at ${offset}`);
}

async function consume(chunk) {
  stats.bytes += chunk.length;
  pending = pending.length ? Buffer.concat([pending, chunk]) : chunk;
  let offset = 0;
  for (;;) {
    const next = decodeOne(pending, offset);
    if (next === null) break;
    offset = next;
  }
  pending = pending.subarray(offset);
  if (lines.length > 512) {
    const out = lines.join('\n') + '\n';
    lines = [];
    await appendFile(logPath, out);
  }
}

// One frame's events become a single slice of the container. The index keeps
// only where that slice is, so the viewer can jump straight to a frame.
function flushFrame(frameIndex) {
  const payload = Buffer.from(JSON.stringify({ events: frameEvents }), 'utf8');
  const slice = stash(payload);
  index.frames.push({
    index: frameIndex,
    calls: frameEvents.filter((e) => e.t === 'call').length,
    ...slice,
    shot: frameShot,
    draws: frameDraws,
  });
  frameEvents = [];
  frameDraws = [];
  frameShot = null;
  pendingDraw = null;
}

// Footer: the index, its length, and the magic again, so a reader can find it
// from the end without scanning.
function closeContainer() {
  if (container === null) return;
  if (frameEvents.length) flushFrame(index.frames.length);
  const body = Buffer.from(JSON.stringify(index), 'utf8');
  const tail = Buffer.alloc(8);
  tail.writeUInt32LE(body.length, 0);
  tail.write('GLTR', 4, 'ascii');
  writeSync(container, body);
  writeSync(container, tail);
  closeSync(container);
  container = null;
}

async function flush() {
  if (!lines.length) return;
  await appendFile(logPath, lines.join('\n') + '\n');
  lines = [];
}

// Minimal server-side WebSocket frame reader: binary frames from a browser are
// always masked, and may be fragmented or split across TCP reads.
function readFrames(socket, onMessage) {
  let buf = Buffer.alloc(0);
  let fragments = [];
  socket.on('data', (chunk) => {
    buf = Buffer.concat([buf, chunk]);
    for (;;) {
      if (buf.length < 2) return;
      const fin = (buf[0] & 0x80) !== 0;
      const opcode = buf[0] & 0x0f;
      const masked = (buf[1] & 0x80) !== 0;
      let len = buf[1] & 0x7f;
      let p = 2;
      if (len === 126) {
        if (buf.length < p + 2) return;
        len = buf.readUInt16BE(p);
        p += 2;
      } else if (len === 127) {
        if (buf.length < p + 8) return;
        len = Number(buf.readBigUInt64BE(p));
        p += 8;
      }
      let mask = null;
      if (masked) {
        if (buf.length < p + 4) return;
        mask = buf.subarray(p, p + 4);
        p += 4;
      }
      if (buf.length < p + len) return;
      const payload = Buffer.from(buf.subarray(p, p + len));
      if (mask) for (let i = 0; i < payload.length; i++) payload[i] ^= mask[i & 3];
      buf = buf.subarray(p + len);

      if (opcode === 0x8) {
        socket.end();
        return;
      }
      if (opcode === 0x9) continue; // ping; the client does not need a pong here
      if (opcode === 0x0 || opcode === 0x1 || opcode === 0x2) {
        fragments.push(payload);
        if (fin) {
          const whole = fragments.length === 1 ? fragments[0] : Buffer.concat(fragments);
          fragments = [];
          onMessage(whole);
        }
      }
    }
  });
}

await mkdir(outDir, { recursive: true });
await writeFile(logPath, '');
// The viewer is written next to the trace so the pair always match.
await writeFile(join(outDir, 'viewer.html'), VIEWER_HTML);
container = openSync(containerPath, 'w');
writeSync(container, Buffer.from('GLTR\x01\x00\x00\x00', 'binary'));
containerAt = 8;

const server = createServer((_, res) => {
  res.writeHead(426);
  res.end('websocket only');
});

server.on('upgrade', (req, socket) => {
  const key = req.headers['sec-websocket-key'];
  if (!key) {
    socket.destroy();
    return;
  }
  const accept = createHash('sha1').update(key + GUID).digest('base64');
  socket.write(
    'HTTP/1.1 101 Switching Protocols\r\n' +
    'Upgrade: websocket\r\n' +
    'Connection: Upgrade\r\n' +
    `Sec-WebSocket-Accept: ${accept}\r\n\r\n`);
  console.log('tracer connected');
  readFrames(socket, (msg) => { consume(msg).catch((e) => console.error(e.message)); });
  socket.on('close', async () => {
    await flush();
    console.log(
      `tracer disconnected: ${stats.calls} calls, ${stats.frames} frames, ` +
      `${stats.errors} GL errors, ${stats.blobs} blobs, ${stats.textures} textures, ` +
      `${stats.dataBytes} data bytes, ` +
      `${stats.labels} labels, ${stats.messages} messages, ` +
      `${stats.shaders} translated shaders, ${stats.shots} shots, ` +
      `${stats.bytes} bytes`);
    closeContainer();
    console.log(`trace written to ${logPath}`);
    console.log(
      `timeline: open ${join(outDir, 'viewer.html')} and drop in ` +
      `${containerPath}`);
  });
});

server.listen(port, '127.0.0.1', () => {
  console.log(`GL trace collector on ws://127.0.0.1:${port}, writing ${outDir}`);
});

for (const sig of ['SIGINT', 'SIGTERM']) {
  process.on(sig, async () => {
    await flush();
    closeContainer();
    console.log(`\n${stats.calls} calls, ${stats.errors} GL errors -> ${logPath}`);
    process.exit(0);
  });
}
