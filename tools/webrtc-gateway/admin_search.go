package main

// The admin interface's Search tab: a query form over the SQLite journal
// (see journal.go), for looking up what happened to a tracking ID after
// its session is gone from the live view.

import (
	"fmt"
	"strings"

	"charm.land/bubbles/v2/table"
	"charm.land/bubbles/v2/textinput"
	tea "charm.land/bubbletea/v2"
	"charm.land/lipgloss/v2"
)

// Focus ring positions, in tab order.
const (
	searchFocusTracking = iota
	searchFocusServer
	searchFocusResults
	searchFocusCount
)

type searchTab struct {
	journal  *Journal
	tracking textinput.Model
	serverID textinput.Model
	results  table.Model
	focus    int
	// status is the line under the form: hit count, or why there are none.
	status string
	width  int
}

func newSearchTab(j *Journal) searchTab {
	tracking := textinput.New()
	tracking.Placeholder = "S-1A2B3C or C-4D5E6F"
	tracking.Prompt = ""
	tracking.CharLimit = 64
	tracking.SetWidth(32)

	serverID := textinput.New()
	serverID.Placeholder = "bloodgulch-ci"
	serverID.Prompt = ""
	serverID.CharLimit = 64
	serverID.SetWidth(32)

	results := table.New(
		table.WithColumns(searchColumns(120)),
		table.WithRows([]table.Row{}),
		table.WithFocused(false),
	)

	s := searchTab{
		journal:  j,
		tracking: tracking,
		serverID: serverID,
		results:  results,
		focus:    searchFocusTracking,
		status:   "enter a tracking ID and/or server ID, then press enter",
	}
	if j == nil {
		s.status = "journaling is disabled -- start the gateway with -journal-db <file> to record sessions"
	}
	s.tracking.Focus()
	return s
}

// searchColumns sizes the detail column off the pane width: the fixed
// columns carry the identity, and detail is the one that benefits from
// whatever room is left.
func searchColumns(width int) []table.Column {
	fixed := 19 + 7 + 22 + 9 + 16 + 10 + 10
	detail := width - fixed - 8
	if detail < 20 {
		detail = 20
	}
	return []table.Column{
		{Title: "Time", Width: 19},
		{Title: "Scope", Width: 7},
		{Title: "Event", Width: 22},
		{Title: "Outcome", Width: 9},
		{Title: "Server", Width: 16},
		{Title: "S-Track", Width: 10},
		{Title: "C-Track", Width: 10},
		{Title: "Detail", Width: detail},
	}
}

func (s *searchTab) setSize(width, height int) {
	s.width = width
	s.results.SetColumns(searchColumns(width))
	s.results.SetWidth(width)
	// Form rows + status + help take the rest.
	h := height - 8
	if h < 3 {
		h = 3
	}
	s.results.SetHeight(h)
	s.tracking.SetWidth(min(40, width/3))
	s.serverID.SetWidth(min(40, width/3))
}

// inputFocused reports whether a text field currently has focus, so the
// parent model knows not to treat a bare "q" as quit.
func (s searchTab) inputFocused() bool {
	return s.focus == searchFocusTracking || s.focus == searchFocusServer
}

func (s *searchTab) setFocus(next int) {
	s.focus = (next + searchFocusCount) % searchFocusCount
	s.tracking.Blur()
	s.serverID.Blur()
	s.results.Blur()
	switch s.focus {
	case searchFocusTracking:
		s.tracking.Focus()
	case searchFocusServer:
		s.serverID.Focus()
	case searchFocusResults:
		s.results.Focus()
	}
}

// run executes the current query and refills the results table.
func (s *searchTab) run() {
	if s.journal == nil {
		s.status = "journaling is disabled -- start the gateway with -journal-db <file>"
		return
	}
	events, err := s.journal.Query(journalFilter{
		trackingID: s.tracking.Value(),
		serverID:   s.serverID.Value(),
	})
	if err != nil {
		s.status = "query failed: " + err.Error()
		s.results.SetRows(nil)
		return
	}

	rows := make([]table.Row, 0, len(events))
	for _, ev := range events {
		rows = append(rows, table.Row{
			ev.ts.Format("2006-01-02 15:04:05"),
			ev.scope,
			ev.event,
			ev.outcome,
			ev.serverID,
			ev.serverTrackingID,
			ev.clientTrackingID,
			ev.detail,
		})
	}
	s.results.SetRows(rows)
	s.results.GotoTop()

	switch {
	case len(rows) == 0:
		s.status = "no matching events"
	case len(rows) == 500:
		// Query's default cap; say so rather than let it read as "that
		// is everything there was".
		s.status = "500 events (result limit reached -- narrow the query)"
	default:
		s.status = fmt.Sprintf("%d events, newest first", len(rows))
	}
}

func (s searchTab) Update(msg tea.Msg) (searchTab, tea.Cmd) {
	if key, ok := msg.(tea.KeyMsg); ok {
		switch key.Key().Code {
		case tea.KeyTab:
			if key.Key().Mod == tea.ModShift {
				s.setFocus(s.focus - 1)
			} else {
				s.setFocus(s.focus + 1)
			}
			return s, nil
		case tea.KeyEnter:
			s.run()
			return s, nil
		case tea.KeyEsc:
			// Leave the fields so the single-key shortcuts work again.
			s.setFocus(searchFocusResults)
			return s, nil
		}
	}

	var cmd tea.Cmd
	switch s.focus {
	case searchFocusTracking:
		s.tracking, cmd = s.tracking.Update(msg)
	case searchFocusServer:
		s.serverID, cmd = s.serverID.Update(msg)
	case searchFocusResults:
		s.results, cmd = s.results.Update(msg)
	}
	return s, cmd
}

func (s searchTab) View() string {
	label := lipgloss.NewStyle().Width(12)
	focused := lipgloss.NewStyle().Foreground(lipgloss.Green)

	trackingLabel := label.Render("Tracking ID")
	serverLabel := label.Render("Server ID")
	if s.focus == searchFocusTracking {
		trackingLabel = focused.Render(label.Render("Tracking ID"))
	}
	if s.focus == searchFocusServer {
		serverLabel = focused.Render(label.Render("Server ID"))
	}

	form := lipgloss.JoinVertical(
		lipgloss.Left,
		trackingLabel+" "+s.tracking.View(),
		serverLabel+" "+s.serverID.View(),
	)

	help := lipgloss.NewStyle().Faint(true).Render(
		"tab/shift+tab: field  enter: search  ctrl+n/ctrl+p: switch tab  ctrl+c: quit")

	return strings.Join([]string{
		form,
		"",
		s.status,
		s.results.View(),
		help,
	}, "\n")
}
