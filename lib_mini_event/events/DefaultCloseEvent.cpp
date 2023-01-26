#include "DefaultCloseEvent.h"
#include "MiniSession.h"

DefaultCloseEvent::DefaultCloseEvent(MiniSession* session) : _session(session) {}

void DefaultCloseEvent::Process() {
    _session->Close();
}
