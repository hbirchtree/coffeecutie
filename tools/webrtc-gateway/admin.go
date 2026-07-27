package main

// An example Bubble Tea server. This will put an ssh session into alt screen
// and continually print up to date terminal information.

import (
	"context"
	"fmt"
	"io"
	"net"
	"os"
	"os/signal"
	"sort"
	"strings"
	"syscall"
	"time"

	// "charm.land/bubbles/v2/help"
	// "charm.land/bubbles/v2/key"
	"charm.land/bubbles/v2/list"
	"charm.land/bubbles/v2/progress"
	"charm.land/bubbles/v2/spinner"
	"charm.land/bubbles/v2/table"
	"charm.land/bubbles/v2/textarea"

	// "charm.land/bubbles/v2/viewport"
	tea "charm.land/bubbletea/v2"
	"charm.land/lipgloss/v2"
	"charm.land/log/v2"
	"charm.land/wish/v2"
	"charm.land/wish/v2/activeterm"
	"charm.land/wish/v2/bubbletea"
	"charm.land/wish/v2/logging"
	"github.com/charmbracelet/ssh"
)

type adminInterfaceOpts struct {
	host       string
	port       string
	privateKey string
}

type adminInterfaceModel struct {
	workingSet *serverWorkingSet
	settings   *serverSettings
}

func startAdminInterface(opts adminInterfaceOpts, model adminInterfaceModel) {
	app := newApp(opts, model)
	app.Start(opts)
}

// app contains a wish server and the list of running programs.
type app struct {
	*ssh.Server
	progs []*tea.Program
	data  *adminInterfaceModel
}

// send dispatches a message to all running programs.
func (a *app) send(msg tea.Msg) {
	for _, p := range a.progs {
		go p.Send(msg)
	}
}

func newApp(opts adminInterfaceOpts, model adminInterfaceModel) *app {
	a := new(app)
	a.data = &model
	s, err := wish.NewServer(
		wish.WithAddress(net.JoinHostPort(opts.host, opts.port)),
		wish.WithHostKeyPath(opts.privateKey),
		wish.WithMiddleware(
			bubbletea.MiddlewareWithProgramHandler(a.ProgramHandler),
			activeterm.Middleware(),
			logging.Middleware(),
		),
	)
	if err != nil {
		log.Error("Could not start server", "error", err)
	}

	a.Server = s
	return a
}

func (a *app) Start(opts adminInterfaceOpts) {
	var err error
	done := make(chan os.Signal, 1)
	signal.Notify(done, os.Interrupt, syscall.SIGINT, syscall.SIGTERM)
	log.Info("Starting SSH server", "host", opts.host, "port", opts.port)
	go func() {
		if err = a.ListenAndServe(); err != nil {
			log.Error("Could not start server", "error", err)
			done <- nil
		}
	}()

	<-done
	log.Info("Stopping SSH server")
	ctx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
	defer func() { cancel() }()
	if err := a.Shutdown(ctx); err != nil {
		log.Error("Could not stop server", "error", err)
	}
	os.Exit(0)
}

func (a *app) ProgramHandler(s ssh.Session) *tea.Program {
	model := initialModel()
	model.app = a
	model.id = s.User()

	model.serverSettings.SetRows(serverConfigItems(model.app.data))
	model.serverList.SetItems(serverListItems(model.app.data.workingSet))

	p := tea.NewProgram(model, bubbletea.MakeOptions(s)...)
	a.progs = append(a.progs, p)

	return p
}

// serverListItems snapshots the registry into list items for serverList,
// sorted by ID -- map iteration order is randomized, and this is rebuilt
// every tick, so without sorting the list would reshuffle each refresh.
func serverListItems(ws *serverWorkingSet) []list.Item {
	items := make([]list.Item, 0)
	ws.servers.RLock()
	for id, server := range ws.servers.registry {
		server.mu.Lock()
		// A webrtc-hosted server has no address of its own -- it is
		// reachable only down its own /server-signal connection.
		addr := "<datachannel>"
		if server.transport != transportWebRTC {
			// Only known once a client's relay punch has arrived.
			addr = "<no client yet>"
			if server.gameAddr != nil {
				addr = server.gameAddr.String()
			}
		}
		server.mu.Unlock()
		items = append(items, serverItem{
			id:         id,
			addr:       addr,
			transport:  server.transport,
			trackingID: server.trackingID,
		})
	}
	ws.servers.RUnlock()
	sort.Slice(items, func(i, j int) bool {
		return items[i].(serverItem).id < items[j].(serverItem).id
	})
	return items
}

func serverConfigItems(model *adminInterfaceModel) []table.Row {
	settings := model.settings
	model.workingSet.relayPortPool.mu.RLock()
	relayPortsUsed := len(model.workingSet.relayPortPool.usedPorts)
	relayPortsTotal := model.workingSet.relayPortPool.maxPort - model.workingSet.relayPortPool.minPort
	model.workingSet.relayPortPool.mu.RUnlock()
	return []table.Row{
		{"Registration TTL", settings.registrationTTL.String()},
		{"Challenge timeout", settings.challengeTimeout.String()},
		{"Relay punch timeout", settings.relayPunchTimeout.String()},
		{"Punch port", fmt.Sprintf("%d", settings.punchPortToAdvertise)},
		// {"WebSocket address", settings.websocketAddr.String()},
		// {"DataChannel address", settings.datachannelAddr.String()},
		{"Challenge address", settings.challengeAddr.String()},
		{"Relay ports", fmt.Sprintf("%d/%d", relayPortsUsed, relayPortsTotal)},
	}
}

const (
	clientColumnStatus = iota
	clientColumnTrackingID
	clientColumnProtocol
)

// currentServerRows snapshots one registry entry's info/client rows for the
// detail panel. ok is false if the server no longer exists (e.g. expired).
// spinnerFrame is the current frame of the shared spinner, stamped into a
// client row's leading status column while that session is still
// establishing (punch/ICE not resolved yet) and left blank once settled.
func currentServerRows(ws *serverWorkingSet, id string, spinnerFrame string) (info []table.Row, clients []table.Row, ok bool) {
	ws.servers.RLock()
	server, exists := ws.servers.registry[id]
	ws.servers.RUnlock()
	if !exists {
		return nil, nil, false
	}

	server.mu.Lock()
	webrtcHosted := server.transport == transportWebRTC
	if webrtcHosted {
		// No UDP leg exists for these: the gateway bridges the client's
		// DataChannel to one this server opens per session, so there is
		// no address, no punch and no challenge to report.
		info = []table.Row{
			{"Tracking ID", server.trackingID},
			{"Transport", "webrtc (datachannel bridge)"},
			{"Game address", "<none -- bridged>"},
			{"Active", fmt.Sprintf("%t", server.active)},
			{"Expiry", server.expiresAt.String()},
		}
	} else {
		// Game address is per-client under symmetric NAT -- this is the
		// last one learned, and is unknown until some client's relay
		// punch lands.
		gameAddrStr := "<no client yet>"
		if server.gameAddr != nil {
			gameAddrStr = server.gameAddr.String()
		}
		info = []table.Row{
			{"Tracking ID", server.trackingID},
			{"Transport", "udp (relay + NAT punch)"},
			{"Game address", gameAddrStr},
			{"Challenge address", server.challengeAddr.String()},
			{"Active", fmt.Sprintf("%t", server.active)},
			{"Expiry", server.expiresAt.String()},
		}
	}
	server.mu.Unlock()

	clients = make([]table.Row, 0)
	ws.clients.RLock()
	for _, client := range ws.clients.sessions {
		if client.serverID != id {
			continue
		}
		client.mu.Lock()
		protocol := client.protocol
		serverAddr := client.serverAddr
		peerLocalPort := client.peerLocalPort
		peerRemoteAddr := client.peerRemoteAddr
		client.mu.Unlock()

		// A bridged session never has a UDP peer address to show.
		serverAddrStr := "<pending punch>"
		if webrtcHosted {
			serverAddrStr = "<datachannel bridge>"
		} else if serverAddr != nil {
			serverAddrStr = serverAddr.String()
		}
		peerLocalStr := "<pending ICE>"
		if peerLocalPort != 0 {
			peerLocalStr = fmt.Sprintf("%d", peerLocalPort)
		}
		peerRemoteStr := "<pending ICE>"
		if peerRemoteAddr != nil {
			peerRemoteStr = peerRemoteAddr.String()
		}

		// Still establishing (punch or ICE unresolved) unless it's
		// already been retired to direct UDP, which is a settled state
		// regardless of these gateway-side fields' now-stale values.
		pending := protocol != "udp" && (serverAddr == nil || peerRemoteAddr == nil)
		statusStr := ""
		if pending {
			statusStr = spinnerFrame
		}

		clients = append(clients, table.Row{
			statusStr,
			client.trackingID,
			protocol,
			peerRemoteStr,
			peerLocalStr,
			fmt.Sprintf("%d", client.relayPort),
			serverAddrStr,
		})
	}
	ws.clients.RUnlock()

	sort.Slice(clients, func(i, j int) bool {
		return clients[i][clientColumnTrackingID] < clients[j][clientColumnTrackingID]
	})

	return info, clients, true
}

// refreshTickInterval is how often the admin TUI re-polls shared state for
// changes made by other goroutines (registrations, heartbeats, expiry).
// Matches spinner.Dot's own FPS (see initialModel) so the per-client
// pending-status spinner in currentClients animates smoothly instead of
// stepping once a second.
var refreshTickInterval = spinner.Dot.FPS

type tickMsg time.Time

func tick() tea.Cmd {
	return tea.Tick(refreshTickInterval, func(t time.Time) tea.Msg {
		return tickMsg(t)
	})
}

type (
	errMsg  error
	chatMsg struct {
		id   string
		text string
	}
)

type styles struct {
	title           lipgloss.Style
	server          lipgloss.Style
	selectedServer  lipgloss.Style
	centerRightPane lipgloss.Style
	help            lipgloss.Style
	quitText        lipgloss.Style
}

var containerStyle = lipgloss.NewStyle().BorderStyle(lipgloss.DoubleBorder()).BorderForeground(lipgloss.White)

// currentInfoRowCount is the fixed number of property rows currentInfo
// always shows (Active/Challenge address/Expiry/Server address). Its
// viewport height is sized off this constant, not off Rows() at layout
// time -- currentInfo is empty until a server is selected, so sizing off
// the live row count would permanently pin the viewport height to zero.
const currentInfoRowCount = 4

type model struct {
	*app
	id             string
	err            error
	styles         *styles
	serverList     list.Model
	serverSettings table.Model

	currentServerId string
	currentInfo     table.Model
	currentClients  table.Model
	currentSpinner  spinner.Model
	currentTime     progress.Model

	rightPaneHeight int
}

type serverItem struct {
	id         string
	addr       string
	transport  string
	trackingID string
}

func (i serverItem) FilterValue() string { return "" }

type serverItemDelegate struct {
	styles *styles
}

func (d serverItemDelegate) Height() int                             { return 1 }
func (d serverItemDelegate) Spacing() int                            { return 0 }
func (d serverItemDelegate) Update(_ tea.Msg, _ *list.Model) tea.Cmd { return nil }
func (d serverItemDelegate) Render(w io.Writer, m list.Model, index int, listItem list.Item) {
	i, ok := listItem.(serverItem)
	if !ok {
		return
	}

	str := fmt.Sprintf("%s %s [%s] (%s)", i.trackingID, i.id, i.transport, i.addr)

	fn := d.styles.server.Render
	if index == m.Index() {
		fn = func(s ...string) string {
			return d.styles.selectedServer.Render(">" + strings.Join(s, " "))
		}
	}

	fmt.Fprint(w, fn(str))
}

func initialModel() model {
	styles := styles{
		title:           lipgloss.NewStyle().MarginLeft(2),
		server:          lipgloss.NewStyle().PaddingLeft(4),
		selectedServer:  lipgloss.NewStyle().PaddingLeft(3).Foreground(lipgloss.Green),
		centerRightPane: lipgloss.NewStyle(),
	}
	serverList := list.New(
		[]list.Item{serverItem{id: "default"}},
		serverItemDelegate{styles: &styles},
		20,
		14,
	)
	serverList.Title = "Registered servers"
	serverSettings := table.New(
		table.WithColumns([]table.Column{
			{Title: "Setting", Width: 20},
			{Title: "Value", Width: 16},
		}),
		table.WithRows([]table.Row{}),
	)

	currentInfo := table.New(
		table.WithColumns([]table.Column{
			{Title: "Property", Width: 16},
			{Title: "Value", Width: 128},
		}),
		table.WithRows([]table.Row{}),
		table.WithFocused(false),
	)
	currentClients := table.New(
		table.WithColumns([]table.Column{
			{Title: "", Width: 3},
			{Title: "Client", Width: 10},
			{Title: "Protocol", Width: 8},
			{Title: "ICE peer address", Width: 24},
			{Title: "ICE local port", Width: 16},
			{Title: "Relay port", Width: 16},
			{Title: "Server address", Width: 24},
		}),
		table.WithRows([]table.Row{}),
		table.WithFocused(false),
	)
	currentTime := progress.New(
		progress.WithColors(lipgloss.Green, lipgloss.Red),
	)
	currentSpinner := spinner.New()
	currentSpinner.Spinner = spinner.Dot
	currentSpinner.Style = lipgloss.NewStyle().Foreground(lipgloss.White)

	return model{
		err:            nil,
		styles:         &styles,
		serverList:     serverList,
		serverSettings: serverSettings,

		currentServerId: "",
		currentInfo:     currentInfo,
		currentClients:  currentClients,
		currentTime:     currentTime,
		currentSpinner:  currentSpinner,
	}
}

func (m model) Init() tea.Cmd {
	return tea.Batch(textarea.Blink, tick(), m.currentSpinner.Tick)
}

func (m model) Update(msg tea.Msg) (tea.Model, tea.Cmd) {
	switch msg := msg.(type) {
	case tea.WindowSizeMsg:
		m.serverList.SetWidth(msg.Width / 3)
		m.serverList.SetHeight(msg.Height - 2 - 1 - (1 + len(m.serverSettings.Rows())))
		m.serverSettings.SetWidth(msg.Width / 3)
		m.serverSettings.SetHeight(1 + len(m.serverSettings.Rows()))
		m.currentTime.SetWidth((msg.Width/3)*2 - 10)
		m.currentInfo.SetWidth((msg.Width/3)*2 - 1)
		m.currentInfo.SetHeight(1 + currentInfoRowCount)
		m.currentClients.SetWidth((msg.Width/3)*2 - 1)
		m.currentClients.SetHeight(msg.Height - m.currentInfo.Height() - 5)
		m.rightPaneHeight = msg.Height - 2
		m.styles.centerRightPane = lipgloss.NewStyle().
			Width((msg.Width / 3) * 2).
			Height(msg.Height - 2).
			AlignHorizontal(lipgloss.Center).
			AlignVertical(lipgloss.Center)
		return m, nil
	case tea.KeyMsg:
		switch msg.Key().Mod {
		case tea.ModCtrl: // We're only interested in ctrl+<key>
			switch msg.Key().Code {
			case 'c':
				return m, tea.Quit
			}
		}
		switch msg.Key().Code {
		case 'q':
			return m, tea.Quit
		}
		if m.currentServerId != "" {
			switch msg.Key().Code {
			case tea.KeyEsc, tea.KeyLeft:
				m.currentServerId = ""
				return m, nil
			}
			m.currentInfo, _ = m.currentInfo.Update(msg)
			return m, nil
		}
		switch msg.Key().Code {
		case tea.KeyEnter, tea.KeyRight:
			i, ok := m.serverList.SelectedItem().(serverItem)
			if ok {
				info, clients, exists := currentServerRows(m.app.data.workingSet, i.id, m.currentSpinner.View())
				if exists {
					m.currentServerId = i.id
					m.currentInfo.SetRows(info)
					m.currentClients.SetRows(clients)
				} else {
					log.Printf("found no server instance")
				}
			}
			return m, nil
		}
	case tickMsg:
		m.serverList.SetItems(serverListItems(m.app.data.workingSet))
		m.serverSettings.SetRows(serverConfigItems(m.app.data))
		if m.currentServerId != "" {
			if info, clients, exists := currentServerRows(m.app.data.workingSet, m.currentServerId, m.currentSpinner.View()); exists {
				m.currentInfo.SetRows(info)
				m.currentClients.SetRows(clients)
			} else {
				m.currentServerId = ""
			}
		}
		return m, tick()
	// We handle errors just like any other message
	case errMsg:
		m.err = msg
		return m, nil
	}
	var spinnerCmd, listCmd tea.Cmd
	m.currentSpinner, spinnerCmd = m.currentSpinner.Update(msg)
	m.serverList, listCmd = m.serverList.Update(msg)

	return m, tea.Batch(spinnerCmd, listCmd)
}

func (m model) View() tea.View {
	var rightPane string
	if m.currentServerId != "" {
		top := lipgloss.JoinVertical(
			lipgloss.Left,
			m.currentInfo.View(),
			m.currentClients.View(),
		)
		status := "  " + m.currentSpinner.View() + m.currentTime.ViewAs(0.7)

		topLines := strings.Split(top, "\n")
		statusLines := strings.Split(status, "\n")
		gap := m.rightPaneHeight - len(topLines) - len(statusLines)
		if gap < 0 {
			gap = 0
		}

		content := make([]string, 0, len(topLines)+gap+len(statusLines))
		content = append(content, topLines...)
		content = append(content, make([]string, gap)...)
		content = append(content, statusLines...)

		rightPane = containerStyle.Render(strings.Join(content, "\n"))
	} else {
		rightPane = containerStyle.Render(m.styles.centerRightPane.Render("Select a server to inspect it"))
	}

	v := tea.NewView(
		lipgloss.JoinHorizontal(
			lipgloss.Top,
			containerStyle.Render(
				fmt.Sprintf("%s\n\n%s",
					m.serverList.View(),
					m.serverSettings.View(),
				),
			),
			rightPane,
		),
	)
	v.AltScreen = true
	return v
}
