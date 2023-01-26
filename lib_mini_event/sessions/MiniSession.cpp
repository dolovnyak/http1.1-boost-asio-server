#include "MiniSession.h"

MiniEvent MiniSession::MakeCloseMiniEvent() {
    return DefaultCloseEvent(this);
}

void MiniSession::Close() {
}
