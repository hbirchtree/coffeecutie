// The timeline viewer for a .gltrace container, kept as a string so the
// collector can drop a matching copy next to every trace it writes.
//
// It reads the footer index first and then slices out only what is on screen,
// so a multi-gigabyte trace scrubs as fast as a small one. The file is opened
// through the File API rather than fetched, which means no server and no CORS.

export const VIEWER_HTML = String.raw`<!doctype html>
<meta charset="utf-8">
<title>GL trace timeline</title>
<style>
  :root {
    color-scheme: dark;
    --bg: #14161a;
    --panel: #1c1f26;
    --line: #2b303b;
    --text: #d8dce4;
    --dim: #858c9b;
    --accent: #6aa9ff;
    --warn: #ff6b6b;
  }
  * { box-sizing: border-box; }
  body {
    margin: 0; background: var(--bg); color: var(--text);
    font: 13px/1.5 ui-monospace, SFMono-Regular, Menlo, Consolas, monospace;
    height: 100vh; display: flex; flex-direction: column;
  }
  header {
    padding: 10px 14px; border-bottom: 1px solid var(--line);
    display: flex; gap: 14px; align-items: center; flex-wrap: wrap;
  }
  h1 { font-size: 13px; margin: 0; font-weight: 600; letter-spacing: .04em; }
  .dim { color: var(--dim); }
  #drop {
    flex: 1; display: flex; align-items: center; justify-content: center;
    border: 1px dashed var(--line); border-radius: 6px; margin: 24px;
    color: var(--dim); text-align: center; padding: 40px;
  }
  #drop.over { border-color: var(--accent); color: var(--accent); }
  #drop[hidden] { display: none; }
  #main { flex: 1; display: none; min-height: 0; }
  #left { width: 45%; display: flex; flex-direction: column; min-width: 0;
          border-right: 1px solid var(--line); }
  #right { flex: 1; display: flex; flex-direction: column; min-width: 0; }
  #shot { flex: 1; display: flex; align-items: center; justify-content: center;
          background: #0c0e12; overflow: hidden; padding: 10px; }
  #shot img { max-width: 100%; max-height: 100%; image-rendering: pixelated;
              border: 1px solid var(--line); }
  #draws { display: flex; gap: 6px; overflow-x: auto; padding: 8px 10px;
           border-top: 1px solid var(--line); background: var(--panel); }
  #draws figure { margin: 0; flex: 0 0 auto; cursor: pointer; text-align: center;
                  border: 1px solid transparent; padding: 2px; border-radius: 4px; }
  #draws figure.sel { border-color: var(--accent); }
  #draws img { display: block; height: 64px; image-rendering: pixelated; }
  #draws figcaption { font-size: 10px; color: var(--dim); margin-top: 2px; }
  #scrub { padding: 10px 14px; border-top: 1px solid var(--line);
           display: flex; gap: 10px; align-items: center; background: var(--panel); }
  #scrub input[type=range] { flex: 1; accent-color: var(--accent); }
  #calls { flex: 1; overflow: auto; padding: 8px 0; }
  .row { padding: 0 12px; white-space: pre; }
  .row:hover { background: #22262f; }
  .fn { color: var(--text); }
  .grp { color: #b9a0ff; }
  .lbl { color: #7fd18c; }
  .msg { color: #e8c07d; }
  .err { color: var(--warn); }
  .meta { color: var(--dim); }
  button { background: #262b35; color: var(--text); border: 1px solid var(--line);
           border-radius: 4px; padding: 4px 10px; cursor: pointer; font: inherit; }
  button:hover { border-color: var(--accent); }
  #filter { background: #262b35; color: var(--text); border: 1px solid var(--line);
            border-radius: 4px; padding: 4px 8px; font: inherit; width: 180px; }
</style>

<header>
  <h1>GL trace timeline</h1>
  <span id="summary" class="dim">no trace loaded</span>
  <input id="filter" placeholder="filter calls" hidden>
</header>

<div id="drop">Drop a <code>.gltrace</code> file here, or click to choose one</div>
<input id="picker" type="file" accept=".gltrace" hidden>

<div id="main">
  <div id="left">
    <div id="calls"></div>
  </div>
  <div id="right">
    <div id="shot"><span class="dim">no capture for this frame</span></div>
    <div id="draws"></div>
  </div>
</div>

<div id="scrub" hidden>
  <button id="prev">&#9664;</button>
  <input id="range" type="range" min="0" max="0" value="0">
  <button id="next">&#9654;</button>
  <span id="pos" class="dim"></span>
</div>

<script>
const el = (id) => document.getElementById(id);
let file = null;
let index = null;
let current = -1;
let selectedDraw = -1;
const urls = [];

// Object URLs pile up quickly when scrubbing, so they are released each time.
function freeUrls() {
  while (urls.length) URL.revokeObjectURL(urls.pop());
}

async function slice(ref) {
  if (!ref) return null;
  return new Uint8Array(await file.slice(ref.off, ref.off + ref.len).arrayBuffer());
}

async function imageUrl(ref) {
  const bytes = await slice(ref);
  if (!bytes) return null;
  const url = URL.createObjectURL(new Blob([bytes], { type: 'image/png' }));
  urls.push(url);
  return url;
}

async function open(f) {
  file = f;
  const tailBytes = new Uint8Array(await f.slice(f.size - 8).arrayBuffer());
  const magic = String.fromCharCode(...tailBytes.slice(4));
  if (magic !== 'GLTR') {
    el('drop').textContent = 'Not a .gltrace file (bad footer)';
    return;
  }
  const len = new DataView(tailBytes.buffer).getUint32(0, true);
  const body = await f.slice(f.size - 8 - len, f.size - 8).text();
  index = JSON.parse(body);

  const draws = index.frames.reduce((n, fr) => n + fr.draws.length, 0);
  const shots = index.frames.filter((fr) => fr.shot).length;
  el('summary').textContent =
    index.frames.length + ' frames, ' + shots + ' frame captures, ' +
    draws + ' draw captures, ' + Object.keys(index.labels).length + ' labels';
  el('drop').hidden = true;
  el('main').style.display = 'flex';
  el('scrub').hidden = false;
  el('filter').hidden = false;
  el('range').max = String(Math.max(0, index.frames.length - 1));
  show(0);
}

function line(cls, text) {
  const d = document.createElement('div');
  d.className = 'row ' + cls;
  d.textContent = text;
  return d;
}

async function show(n) {
  if (!index || n < 0 || n >= index.frames.length) return;
  current = n;
  selectedDraw = -1;
  freeUrls();
  const fr = index.frames[n];
  el('range').value = String(n);
  el('pos').textContent = 'frame ' + fr.index + ' of ' + index.frames.length +
    ' — ' + fr.calls + ' calls';

  const shotBox = el('shot');
  shotBox.innerHTML = '';
  if (fr.shot) {
    const img = document.createElement('img');
    img.src = await imageUrl(fr.shot);
    shotBox.appendChild(img);
  } else {
    shotBox.innerHTML = '<span class="dim">no capture for this frame</span>';
  }

  const strip = el('draws');
  strip.innerHTML = '';
  for (const d of fr.draws) {
    const fig = document.createElement('figure');
    const img = document.createElement('img');
    img.src = await imageUrl(d.shot);
    const cap = document.createElement('figcaption');
    cap.textContent = '#' + d.n + ' ' + (d.fn || '').replace('gl', '');
    fig.append(img, cap);
    fig.onclick = () => {
      selectedDraw = d.n;
      shotBox.innerHTML = '';
      const big = document.createElement('img');
      big.src = img.src;
      shotBox.appendChild(big);
      for (const other of strip.children) other.classList.remove('sel');
      fig.classList.add('sel');
    };
    strip.appendChild(fig);
  }
  strip.style.display = fr.draws.length ? 'flex' : 'none';

  const bytes = await slice(fr);
  const events = JSON.parse(new TextDecoder().decode(bytes)).events;
  renderCalls(events);
}

function renderCalls(events) {
  const box = el('calls');
  box.innerHTML = '';
  const needle = el('filter').value.trim().toLowerCase();
  let depth = 0;
  const frag = document.createDocumentFragment();
  for (const e of events) {
    const pad = '  '.repeat(depth);
    if (e.t === 'push') {
      if (!needle) frag.appendChild(line('grp', pad + '▸ ' + e.name));
      depth++;
    } else if (e.t === 'pop') {
      if (depth > 0) depth--;
    } else if (e.t === 'label') {
      if (!needle) frag.appendChild(line('lbl', pad + '[label ' + e.handle + ' = "' + e.name + '"]'));
    } else if (e.t === 'msg') {
      if (!needle) frag.appendChild(line('msg', pad + '[message] ' + e.text));
    } else if (e.t === 'call') {
      const text = e.fn + '(' + e.args.join(', ') + ')' +
        (e.size ? ' data=' + e.size + 'B' : '') +
        (e.label ? ' // ' + e.label : '');
      if (needle && !text.toLowerCase().includes(needle)) continue;
      frag.appendChild(line(e.err ? 'err' : 'fn', pad + text +
        (e.err ? '  !! 0x' + e.err.toString(16) : '')));
    }
  }
  box.appendChild(frag);
}

el('drop').onclick = () => el('picker').click();
el('picker').onchange = (e) => e.target.files[0] && open(e.target.files[0]);
document.addEventListener('dragover', (e) => {
  e.preventDefault();
  el('drop').classList.add('over');
});
document.addEventListener('dragleave', () => el('drop').classList.remove('over'));
document.addEventListener('drop', (e) => {
  e.preventDefault();
  el('drop').classList.remove('over');
  if (e.dataTransfer.files[0]) open(e.dataTransfer.files[0]);
});
el('range').oninput = (e) => show(Number(e.target.value));
el('prev').onclick = () => show(current - 1);
el('next').onclick = () => show(current + 1);
el('filter').oninput = () => show(current);
document.addEventListener('keydown', (e) => {
  if (e.target === el('filter')) return;
  if (e.key === 'ArrowLeft') show(current - 1);
  if (e.key === 'ArrowRight') show(current + 1);
});
</script>
`;
