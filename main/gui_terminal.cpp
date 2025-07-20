#include <gtkmm.h>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <memory>

class TerminalWindow : public Gtk::Window {
public:
    TerminalWindow();

protected:
    // Signal handlers
    void on_run_button_clicked();

    // Helper to run shell commands
    std::string run_command(const std::string& cmd);

    // Widgets
    Gtk::Box m_VBox;
    Gtk::TextView m_OutputTextView;
    Gtk::ScrolledWindow m_ScrolledWindow;
    Gtk::Entry m_CommandEntry;
    Gtk::Button m_RunButton;
};

TerminalWindow::TerminalWindow()
    : m_VBox(Gtk::ORIENTATION_VERTICAL),
      m_RunButton("Run")
{
    set_title("Mini GUI Terminal");
    set_default_size(600, 400);
    add(m_VBox);

    // Add Entry + Button
    Gtk::Box* hbox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
    hbox->pack_start(m_CommandEntry, Gtk::PACK_EXPAND_WIDGET);
    hbox->pack_start(m_RunButton, Gtk::PACK_SHRINK);
    m_VBox.pack_start(*hbox, Gtk::PACK_SHRINK);

    // Setup text area
    m_ScrolledWindow.add(m_OutputTextView);
    m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    m_VBox.pack_start(m_ScrolledWindow);

    m_OutputTextView.set_editable(false);

    // Connect button signal
    m_RunButton.signal_clicked().connect(sigc::mem_fun(*this, &TerminalWindow::on_run_button_clicked));

    show_all_children();
}

std::string TerminalWindow::run_command(const std::string& cmd) {
    std::string result;
    char buffer[128];
    std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) return "Failed to run command.\n";

    while (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr) {
        result += buffer;
    }
    return result;
}

void TerminalWindow::on_run_button_clicked() {
    std::string command = m_CommandEntry.get_text();
    if (command.empty()) return;

    std::string output = run_command(command);
    m_OutputTextView.get_buffer()->insert_at_cursor(">>> " + command + "\n" + output + "\n");

    m_CommandEntry.set_text("");
}

int main(int argc, char* argv[]) {
    auto app = Gtk::Application::create(argc, argv, "org.example.miniterminal");

    TerminalWindow window;
    return app->run(window);
}

