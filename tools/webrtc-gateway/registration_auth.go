package main

import (
	"bytes"
	"crypto/ed25519"
	"encoding/base64"
	"encoding/json"
	"errors"
	"fmt"
	"sort"
	"strconv"
	"sync"
	"time"
)

// A serverId is a name anyone can type, so on its own it is first come,
// first served. Binding it to the server's Ed25519 key -- the one its GNS
// certificate and metadata are signed with -- means a registration has to
// prove it holds that key, and once an ID has been claimed under a key no
// other key can register it for as long as the binding is remembered.
//
// The gateway only ever verifies against this key. It never hands it to a
// client as something to trust; clients get it out of band, in the join
// string, and pin it themselves.

const registerSignaturePrefix = "coffee-gateway-register-v1\n"

func parseServerPublicKey(b64 string) (ed25519.PublicKey, error) {
	if b64 == "" {
		return nil, errors.New("missing publicKey")
	}
	raw, err := base64.StdEncoding.DecodeString(b64)
	if err != nil {
		return nil, fmt.Errorf("publicKey is not valid base64: %w", err)
	}
	if len(raw) != ed25519.PublicKeySize {
		return nil, fmt.Errorf("publicKey is %d bytes, want %d", len(raw), ed25519.PublicKeySize)
	}
	return ed25519.PublicKey(raw), nil
}

// registrationChallenge is what the server signs: the nonce it received
// (over UDP for a relay server, over the websocket for a WebRTC-hosted one)
// bound to the ID it is claiming, so a signature cannot be replayed for
// another name.
func registrationChallenge(serverID string, nonce []byte) []byte {
	msg := make([]byte, 0, len(registerSignaturePrefix)+len(serverID)+1+len(nonce))
	msg = append(msg, registerSignaturePrefix...)
	msg = append(msg, serverID...)
	msg = append(msg, '\n')
	return append(msg, nonce...)
}

func verifyRegistrationSignature(pub ed25519.PublicKey, serverID string, nonce []byte, sigB64 string) error {
	if sigB64 == "" {
		return errors.New("missing signature")
	}
	sig, err := base64.StdEncoding.DecodeString(sigB64)
	if err != nil {
		return fmt.Errorf("signature is not valid base64: %w", err)
	}
	if len(sig) != ed25519.SignatureSize || !ed25519.Verify(pub, registrationChallenge(serverID, nonce), sig) {
		return errors.New("signature does not verify against the registered key")
	}
	return nil
}

// idOwnerTable remembers which key last proved ownership of each serverId.
type idBinding struct {
	key      ed25519.PublicKey
	lastSeen time.Time
}

type idOwnerTable struct {
	mu     sync.Mutex
	owners map[string]idBinding
	// memory is how long a binding outlives its registration; 0 keeps it
	// for the life of the process.
	memory time.Duration
}

func newIDOwnerTable(memory time.Duration) *idOwnerTable {
	return &idOwnerTable{owners: map[string]idBinding{}, memory: memory}
}

func (t *idOwnerTable) expired(b idBinding, now time.Time) bool {
	return t.memory > 0 && now.Sub(b.lastSeen) > t.memory
}

// check refuses a key other than the one the ID is bound to.
func (t *idOwnerTable) check(id string, key ed25519.PublicKey, now time.Time) error {
	t.mu.Lock()
	defer t.mu.Unlock()
	b, ok := t.owners[id]
	if !ok {
		return nil
	}
	if t.expired(b, now) {
		delete(t.owners, id)
		return nil
	}
	if !bytes.Equal(b.key, key) {
		return fmt.Errorf("serverId %q is bound to a different key", id)
	}
	return nil
}

func (t *idOwnerTable) claim(id string, key ed25519.PublicKey, now time.Time) {
	t.mu.Lock()
	defer t.mu.Unlock()
	t.owners[id] = idBinding{key: key, lastSeen: now}
}

// touch restarts the memory window, so the window counts from when the
// server was last seen and a server that stays registered keeps its name.
func (t *idOwnerTable) touch(id string, now time.Time) {
	t.mu.Lock()
	defer t.mu.Unlock()
	if b, ok := t.owners[id]; ok {
		b.lastSeen = now
		t.owners[id] = b
	}
}

// Metadata ---------------------------------------------------------------

// canonicalJSON re-serializes a decoded document the way the server signed
// it: nlohmann::json's dump() of an object with recursively sorted keys.
// That is compact output, keys in byte order, numbers verbatim (decode with
// UseNumber) and strings with nlohmann's escaping -- not encoding/json's,
// which differs on \b, \f, <, >, & and U+2028/9 and would fail every
// signature over a string containing one.
func canonicalJSON(v any) ([]byte, error) {
	var buf bytes.Buffer
	if err := writeCanonical(&buf, v); err != nil {
		return nil, err
	}
	return buf.Bytes(), nil
}

func writeCanonical(buf *bytes.Buffer, v any) error {
	switch x := v.(type) {
	case nil:
		buf.WriteString("null")
	case bool:
		buf.WriteString(strconv.FormatBool(x))
	case json.Number:
		buf.WriteString(x.String())
	case string:
		writeNlohmannString(buf, x)
	case []any:
		buf.WriteByte('[')
		for i, item := range x {
			if i > 0 {
				buf.WriteByte(',')
			}
			if err := writeCanonical(buf, item); err != nil {
				return err
			}
		}
		buf.WriteByte(']')
	case map[string]any:
		keys := make([]string, 0, len(x))
		for k := range x {
			keys = append(keys, k)
		}
		sort.Strings(keys)
		buf.WriteByte('{')
		for i, k := range keys {
			if i > 0 {
				buf.WriteByte(',')
			}
			writeNlohmannString(buf, k)
			buf.WriteByte(':')
			if err := writeCanonical(buf, x[k]); err != nil {
				return err
			}
		}
		buf.WriteByte('}')
	default:
		return fmt.Errorf("unexpected JSON value of type %T", v)
	}
	return nil
}

// writeNlohmannString mirrors nlohmann::detail::serializer::dump_escaped
// with ensure_ascii off: short escapes for the common control characters,
// \u00xx in lower-case hex for the rest below 0x20, everything else raw.
func writeNlohmannString(buf *bytes.Buffer, s string) {
	buf.WriteByte('"')
	for i := 0; i < len(s); i++ {
		c := s[i]
		switch c {
		case '"':
			buf.WriteString(`\"`)
		case '\\':
			buf.WriteString(`\\`)
		case '\b':
			buf.WriteString(`\b`)
		case '\f':
			buf.WriteString(`\f`)
		case '\n':
			buf.WriteString(`\n`)
		case '\r':
			buf.WriteString(`\r`)
		case '\t':
			buf.WriteString(`\t`)
		default:
			if c < 0x20 {
				fmt.Fprintf(buf, `\u%04x`, c)
			} else {
				buf.WriteByte(c)
			}
		}
	}
	buf.WriteByte('"')
}

// verifyMetadataSignature checks a metadata payload the way a client
// holding the key would: the "auth" object is removed, the rest
// canonicalized, and the Ed25519 signature verified over those bytes with
// the key the registration proved.
func verifyMetadataSignature(raw []byte, pub ed25519.PublicKey) error {
	dec := json.NewDecoder(bytes.NewReader(raw))
	dec.UseNumber()
	var doc any
	if err := dec.Decode(&doc); err != nil {
		return fmt.Errorf("metadata is not valid JSON: %w", err)
	}
	obj, ok := doc.(map[string]any)
	if !ok {
		return errors.New("metadata is not a JSON object")
	}
	auth, ok := obj["auth"].(map[string]any)
	if !ok {
		return errors.New("metadata carries no auth object")
	}
	if typ, _ := auth["type"].(string); typ != "ed25519" {
		return fmt.Errorf("metadata auth type %q, want ed25519", typ)
	}
	if pkB64, _ := auth["public_key"].(string); pkB64 != "" {
		pk, err := parseServerPublicKey(pkB64)
		if err != nil {
			return fmt.Errorf("metadata auth.public_key: %w", err)
		}
		if !bytes.Equal(pk, pub) {
			return errors.New("metadata names a key other than the registered one")
		}
	}
	sigB64, _ := auth["signature"].(string)
	sig, err := base64.StdEncoding.DecodeString(sigB64)
	if err != nil || len(sig) != ed25519.SignatureSize {
		return errors.New("metadata auth.signature is malformed")
	}
	delete(obj, "auth")
	canonical, err := canonicalJSON(obj)
	if err != nil {
		return err
	}
	if !ed25519.Verify(pub, canonical, sig) {
		return errors.New("metadata signature does not verify against the registered key")
	}
	return nil
}
