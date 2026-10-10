package main

import (
	"crypto/subtle"
	"fmt"
	"net"
	"net/http"
	"net/url"
	"strings"
	"sync"
)

// Everything an unauthenticated peer can spend on this gateway is bounded
// here: which browser origins may open signaling sockets, how a client's
// address is established behind the TLS-terminating proxy, and how many
// sessions and registrations one address may hold at once.

// parseAddrList accepts a comma-separated list of CIDRs or bare addresses.
func parseAddrList(list string) ([]*net.IPNet, error) {
	var out []*net.IPNet
	for _, item := range strings.Split(list, ",") {
		item = strings.TrimSpace(item)
		if item == "" {
			continue
		}
		if !strings.Contains(item, "/") {
			ip := net.ParseIP(item)
			if ip == nil {
				return nil, fmt.Errorf("bad address %q", item)
			}
			bits := 32
			if ip.To4() == nil {
				bits = 128
			}
			item = fmt.Sprintf("%s/%d", ip, bits)
		}
		_, n, err := net.ParseCIDR(item)
		if err != nil {
			return nil, fmt.Errorf("bad address %q: %w", item, err)
		}
		out = append(out, n)
	}
	return out, nil
}

func ipInList(ip net.IP, list []*net.IPNet) bool {
	for _, n := range list {
		if n.Contains(ip) {
			return true
		}
	}
	return false
}

func remoteIP(r *http.Request) net.IP {
	host, _, err := net.SplitHostPort(r.RemoteAddr)
	if err != nil {
		host = r.RemoteAddr
	}
	return net.ParseIP(host)
}

func fromTrustedProxy(r *http.Request) bool {
	ip := remoteIP(r)
	return ip != nil && ipInList(ip, settings.trustedProxies)
}

// clientAddr is the address limits and logs are keyed on. Behind the
// TLS-terminating proxy every connection arrives from the proxy, so the
// client is what the proxy put in X-Forwarded-For -- but only when the
// connection really came from a proxy named in -trusted-proxy. Anyone else
// gets no say in who they are.
func clientAddr(r *http.Request) string {
	ip := remoteIP(r)
	if ip == nil {
		return r.RemoteAddr
	}
	if !ipInList(ip, settings.trustedProxies) {
		return ip.String()
	}
	// Each proxy appends the address it accepted from, so the rightmost
	// entry that is not itself a trusted proxy is the one our proxy saw.
	if fwd := r.Header.Get("X-Forwarded-For"); fwd != "" {
		parts := strings.Split(fwd, ",")
		for i := len(parts) - 1; i >= 0; i-- {
			candidate := net.ParseIP(strings.TrimSpace(parts[i]))
			if candidate == nil {
				break
			}
			if !ipInList(candidate, settings.trustedProxies) {
				return candidate.String()
			}
		}
	}
	if real := net.ParseIP(strings.TrimSpace(r.Header.Get("X-Real-IP"))); real != nil {
		return real.String()
	}
	return ip.String()
}

// signalOrigin is clientAddr for log lines, keeping the proxy's own address
// visible so a wrong -trusted-proxy shows up rather than hiding.
func signalOrigin(r *http.Request) string {
	client := clientAddr(r)
	if ip := remoteIP(r); ip != nil && ip.String() != client {
		return client + " (via " + r.RemoteAddr + ")"
	}
	return r.RemoteAddr
}

// effectiveHost is the host the browser connected to: the proxy's
// X-Forwarded-Host when the connection came through a trusted proxy,
// otherwise the Host header.
func effectiveHost(r *http.Request) string {
	if fromTrustedProxy(r) {
		if fwd := r.Header.Get("X-Forwarded-Host"); fwd != "" {
			return strings.TrimSpace(strings.Split(fwd, ",")[0])
		}
	}
	return r.Host
}

func hostnameOf(hostport string) string {
	if h, _, err := net.SplitHostPort(hostport); err == nil {
		return h
	}
	return strings.Trim(hostport, "[]")
}

// originRule is one -allowed-origins entry, scheme://host[:port]. Without a
// port it matches any port on that host, which is what a page served from
// an ephemeral port (the CI harness) needs.
type originRule struct {
	scheme, host, port string
}

func parseAllowedOrigins(list string) (rules []originRule, allowAll bool, err error) {
	for _, item := range strings.Split(list, ",") {
		item = strings.TrimSpace(item)
		if item == "" {
			continue
		}
		if item == "*" {
			allowAll = true
			continue
		}
		u, err := url.Parse(item)
		if err != nil || u.Scheme == "" || u.Host == "" || u.Path != "" || u.RawQuery != "" {
			return nil, false, fmt.Errorf("bad origin %q: want scheme://host[:port]", item)
		}
		rules = append(rules, originRule{
			scheme: strings.ToLower(u.Scheme),
			host:   strings.ToLower(u.Hostname()),
			port:   u.Port(),
		})
	}
	return rules, allowAll, nil
}

func (rule originRule) matches(u *url.URL) bool {
	if !strings.EqualFold(u.Scheme, rule.scheme) || !strings.EqualFold(u.Hostname(), rule.host) {
		return false
	}
	return rule.port == "" || rule.port == u.Port()
}

// checkOrigin decides whether a browser page may open a signaling socket.
// Browsers always send Origin; native peers (libdatachannel, Go) send none
// and are let through -- the header protects browser users against
// cross-site use of their connection, it does not authenticate the peer.
// With no -allowed-origins the page must come from the same host the
// gateway is reached as, on any port and scheme, which keeps a local page
// against a local gateway working without flags.
func checkOrigin(r *http.Request) bool {
	origin := r.Header.Get("Origin")
	if origin == "" || settings.allowAllOrigins {
		return true
	}
	u, err := url.Parse(origin)
	if err != nil || u.Host == "" {
		return false
	}
	if len(settings.allowedOrigins) > 0 {
		for _, rule := range settings.allowedOrigins {
			if rule.matches(u) {
				return true
			}
		}
		return false
	}
	return strings.EqualFold(u.Hostname(), hostnameOf(effectiveHost(r)))
}

// hostTokenMatches is the one check between a host-role dial and a session's
// bridge slot; the token is per session and only ever told to the server.
func hostTokenMatches(session *clientSession, token string) bool {
	return token != "" &&
		subtle.ConstantTimeCompare([]byte(token), []byte(session.hostToken)) == 1
}

// ipCounter caps how much of something one address, and everyone together,
// may hold open. 0 for either limit means unlimited.
type ipCounter struct {
	mu    sync.Mutex
	perIP map[string]int
	total int
}

func newIPCounter() *ipCounter {
	return &ipCounter{perIP: map[string]int{}}
}

func (c *ipCounter) tryAcquire(ip string, perIPMax, totalMax int) bool {
	c.mu.Lock()
	defer c.mu.Unlock()
	if (perIPMax > 0 && c.perIP[ip] >= perIPMax) || (totalMax > 0 && c.total >= totalMax) {
		return false
	}
	c.perIP[ip]++
	c.total++
	return true
}

func (c *ipCounter) release(ip string) {
	c.mu.Lock()
	defer c.mu.Unlock()
	if c.perIP[ip] <= 1 {
		delete(c.perIP, ip)
	} else {
		c.perIP[ip]--
	}
	if c.total > 0 {
		c.total--
	}
}
