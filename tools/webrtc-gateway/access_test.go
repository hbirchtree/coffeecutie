package main

import (
	"net/http"
	"net/http/httptest"
	"testing"
)

func withSettings(t *testing.T, fn func()) {
	saved := settings
	t.Cleanup(func() { settings = saved })
	fn()
}

func TestClientAddrTrustsOnlyConfiguredProxies(t *testing.T) {
	withSettings(t, func() {
		var err error
		settings.trustedProxies, err = parseAddrList("10.0.0.1, 192.168.0.0/16")
		if err != nil {
			t.Fatal(err)
		}
		for _, tc := range []struct{ remote, xff, realIP, want string }{
			{"203.0.113.9:4000", "198.51.100.7", "", "203.0.113.9"},
			{"10.0.0.1:4000", "198.51.100.7", "", "198.51.100.7"},
			{"10.0.0.1:4000", "198.51.100.7, 192.168.1.5", "", "198.51.100.7"},
			{"10.0.0.1:4000", "", "198.51.100.8", "198.51.100.8"},
			{"10.0.0.1:4000", "garbage", "", "10.0.0.1"},
			{"[2001:db8::1]:4000", "198.51.100.7", "", "2001:db8::1"},
		} {
			r := httptest.NewRequest(http.MethodGet, "/signal", nil)
			r.RemoteAddr = tc.remote
			if tc.xff != "" {
				r.Header.Set("X-Forwarded-For", tc.xff)
			}
			if tc.realIP != "" {
				r.Header.Set("X-Real-IP", tc.realIP)
			}
			if got := clientAddr(r); got != tc.want {
				t.Errorf("remote=%s xff=%q real=%q: got %s want %s", tc.remote, tc.xff, tc.realIP, got, tc.want)
			}
		}
		if _, err := parseAddrList("not-an-address"); err == nil {
			t.Error("garbage address accepted")
		}
	})
}

func TestCheckOrigin(t *testing.T) {
	withSettings(t, func() {
		settings.trustedProxies, _ = parseAddrList("10.0.0.1")
		req := func(origin, host, remote, fwdHost string) *http.Request {
			r := httptest.NewRequest(http.MethodGet, "/signal", nil)
			r.Host = host
			r.RemoteAddr = remote
			if origin != "" {
				r.Header.Set("Origin", origin)
			}
			if fwdHost != "" {
				r.Header.Set("X-Forwarded-Host", fwdHost)
			}
			return r
		}
		settings.allowedOrigins, settings.allowAllOrigins = nil, false
		for _, tc := range []struct {
			name string
			r    *http.Request
			want bool
		}{
			{"no origin (native peer)", req("", "gw.example:8088", "203.0.113.9:1", ""), true},
			{"same host, other port", req("http://127.0.0.1:34567", "127.0.0.1:8098", "127.0.0.1:1", ""), true},
			{"other host", req("https://evil.example", "gw.example", "203.0.113.9:1", ""), false},
			{"null origin", req("null", "gw.example", "203.0.113.9:1", ""), false},
			{"forwarded host from trusted proxy", req("https://game.example", "10.0.0.2:8088", "10.0.0.1:1", "game.example"), true},
			{"forwarded host from untrusted peer", req("https://game.example", "10.0.0.2:8088", "203.0.113.9:1", "game.example"), false},
		} {
			if got := checkOrigin(tc.r); got != tc.want {
				t.Errorf("%s: got %v want %v", tc.name, got, tc.want)
			}
		}
		var err error
		settings.allowedOrigins, settings.allowAllOrigins, err = parseAllowedOrigins("https://game.example, http://127.0.0.1")
		if err != nil {
			t.Fatal(err)
		}
		for _, tc := range []struct {
			name string
			r    *http.Request
			want bool
		}{
			{"listed", req("https://game.example", "gw.example", "203.0.113.9:1", ""), true},
			{"listed, wrong scheme", req("http://game.example", "gw.example", "203.0.113.9:1", ""), false},
			{"listed host, any port", req("http://127.0.0.1:40000", "gw.example", "203.0.113.9:1", ""), true},
			{"same host but not listed", req("https://gw.example", "gw.example", "203.0.113.9:1", ""), false},
		} {
			if got := checkOrigin(tc.r); got != tc.want {
				t.Errorf("%s: got %v want %v", tc.name, got, tc.want)
			}
		}
		if _, _, err := parseAllowedOrigins("game.example"); err == nil {
			t.Error("origin without scheme accepted")
		}
		if _, all, _ := parseAllowedOrigins("*"); !all {
			t.Error("* did not allow all")
		}
	})
}

func TestIPCounterLimits(t *testing.T) {
	c := newIPCounter()
	for i := 0; i < 2; i++ {
		if !c.tryAcquire("a", 2, 3) {
			t.Fatalf("acquire %d for a refused", i)
		}
	}
	if c.tryAcquire("a", 2, 3) {
		t.Fatal("per-address cap not enforced")
	}
	if !c.tryAcquire("b", 2, 3) {
		t.Fatal("b refused while under the total")
	}
	if c.tryAcquire("c", 2, 3) {
		t.Fatal("total cap not enforced")
	}
	c.release("a")
	if !c.tryAcquire("c", 2, 3) {
		t.Fatal("release did not free a slot")
	}
	if !c.tryAcquire("z", 0, 0) {
		t.Fatal("0 must mean unlimited")
	}
}

func TestHostTokenMatches(t *testing.T) {
	s := &clientSession{hostToken: "0123456789abcdef0123456789abcdef"}
	if !hostTokenMatches(s, s.hostToken) {
		t.Fatal("right token refused")
	}
	if hostTokenMatches(s, "") || hostTokenMatches(s, "0123456789abcdef0123456789abcdee") {
		t.Fatal("wrong token accepted")
	}
}
