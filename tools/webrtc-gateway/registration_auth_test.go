package main

import (
	"crypto/ed25519"
	"crypto/rand"
	"encoding/base64"
	"encoding/json"
	"strings"
	"testing"
	"time"
)

func testKey(t *testing.T) (ed25519.PublicKey, ed25519.PrivateKey, string) {
	pub, priv, err := ed25519.GenerateKey(rand.Reader)
	if err != nil {
		t.Fatal(err)
	}
	return pub, priv, base64.StdEncoding.EncodeToString(pub)
}

func TestRegistrationSignature(t *testing.T) {
	pub, priv, _ := testKey(t)
	nonce := []byte("0123456789abcdef")
	sig := base64.StdEncoding.EncodeToString(ed25519.Sign(priv, registrationChallenge("srv", nonce)))
	if err := verifyRegistrationSignature(pub, "srv", nonce, sig); err != nil {
		t.Fatalf("good signature refused: %v", err)
	}
	if verifyRegistrationSignature(pub, "other", nonce, sig) == nil {
		t.Fatal("signature accepted for another serverId")
	}
	if verifyRegistrationSignature(pub, "srv", []byte("fedcba9876543210"), sig) == nil {
		t.Fatal("signature accepted for another nonce")
	}
	otherPub, _, _ := testKey(t)
	if verifyRegistrationSignature(otherPub, "srv", nonce, sig) == nil {
		t.Fatal("signature accepted under another key")
	}
	if verifyRegistrationSignature(pub, "srv", nonce, "") == nil {
		t.Fatal("missing signature accepted")
	}
	if verifyRegistrationSignature(pub, "srv", nonce, "not base64!") == nil {
		t.Fatal("garbage signature accepted")
	}
}

func TestParseServerPublicKey(t *testing.T) {
	_, _, b64 := testKey(t)
	if _, err := parseServerPublicKey(b64); err != nil {
		t.Fatal(err)
	}
	for _, bad := range []string{"", "AAAA", base64.StdEncoding.EncodeToString(make([]byte, 31)), "!!!"} {
		if _, err := parseServerPublicKey(bad); err == nil {
			t.Errorf("%q accepted", bad)
		}
	}
}

func TestServerIDOwnership(t *testing.T) {
	pub, _, _ := testKey(t)
	other, _, _ := testKey(t)
	now := time.Now()
	table := newIDOwnerTable(time.Hour)
	if err := table.check("a", pub, now); err != nil {
		t.Fatal("unclaimed id refused")
	}
	table.claim("a", pub, now)
	if err := table.check("a", pub, now.Add(time.Minute)); err != nil {
		t.Fatal("owner refused its own id")
	}
	if err := table.check("a", other, now.Add(time.Minute)); err == nil {
		t.Fatal("another key took a bound id")
	}
	table.touch("a", now.Add(50*time.Minute))
	if err := table.check("a", other, now.Add(100*time.Minute)); err == nil {
		t.Fatal("binding expired despite being touched")
	}
	if err := table.check("a", other, now.Add(3*time.Hour)); err != nil {
		t.Fatal("binding never expired")
	}
	forever := newIDOwnerTable(0)
	forever.claim("b", pub, now)
	if err := forever.check("b", other, now.Add(1000*time.Hour)); err == nil {
		t.Fatal("memory 0 must mean the binding never expires")
	}
}

// What nlohmann::json's dump() produces for the same document, which is the
// exact byte string the server signs.
func TestCanonicalJSONMatchesNlohmannDump(t *testing.T) {
	raw := `{"player_count":3,"map":"blood\tgulch \"x\"","nested":{"z":[1,2.5,true,null],"a":"<&>é"},"ctl":"\u0001\u001f"}`
	dec := json.NewDecoder(strings.NewReader(raw))
	dec.UseNumber()
	var doc any
	if err := dec.Decode(&doc); err != nil {
		t.Fatal(err)
	}
	got, err := canonicalJSON(doc)
	if err != nil {
		t.Fatal(err)
	}
	want := `{"ctl":"\u0001\u001f","map":"blood\tgulch \"x\"","nested":{"a":"<&>é","z":[1,2.5,true,null]},"player_count":3}`
	if string(got) != want {
		t.Fatalf("canonical mismatch:\n got %s\nwant %s", got, want)
	}
}

func TestVerifyMetadataSignature(t *testing.T) {
	pub, priv, pubB64 := testKey(t)
	body := map[string]any{
		"map": "bloodgulch", "map_type": "multiplayer",
		"player_count": json.Number("3"), "player_count_max": json.Number("16"),
		"identity": "ed25519:abc",
	}
	canonical, err := canonicalJSON(body)
	if err != nil {
		t.Fatal(err)
	}
	sig := ed25519.Sign(priv, canonical)
	withAuth := map[string]any{}
	for k, v := range body {
		withAuth[k] = v
	}
	withAuth["auth"] = map[string]any{
		"type": "ed25519", "public_key": pubB64,
		"signature": base64.StdEncoding.EncodeToString(sig),
	}
	// Different spacing than the canonical form, to prove it is
	// re-canonicalized rather than compared as sent.
	raw, _ := json.MarshalIndent(withAuth, "", "  ")
	if err := verifyMetadataSignature(raw, pub); err != nil {
		t.Fatalf("good metadata refused: %v", err)
	}
	other, _, _ := testKey(t)
	if verifyMetadataSignature(raw, other) == nil {
		t.Fatal("metadata accepted under a different registered key")
	}
	tampered := []byte(strings.Replace(string(raw), `"bloodgulch"`, `"wizard"`, 1))
	if verifyMetadataSignature(tampered, pub) == nil {
		t.Fatal("tampered metadata accepted")
	}
	if verifyMetadataSignature([]byte(`{"map":"x"}`), pub) == nil {
		t.Fatal("unsigned metadata accepted")
	}
	if verifyMetadataSignature([]byte(`not json`), pub) == nil {
		t.Fatal("garbage accepted")
	}
}
