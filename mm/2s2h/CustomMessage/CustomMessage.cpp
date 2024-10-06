
#include "CustomMessage.h"
#include <libultraship/bridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include <map>

extern "C" {
#include "variables.h"
extern f32 sNESFontWidths[160];
}

CustomMessage::Entry activeCustomMessage;

void CustomMessage::StartTextbox(std::string msg, CustomMessage::Entry options) {
    options.msg = msg;
    activeCustomMessage = options;

    Message_StartTextbox(gPlayState, CUSTOM_MESSAGE_ID, &GET_PLAYER(gPlayState)->actor);
}

void CustomMessage::SetActiveCustomMessage(std::string msg, CustomMessage::Entry options) {
    options.msg = msg;
    activeCustomMessage = options;
}

void CustomMessage::Replace(std::string* msg, const std::string& placeholder, const std::string& value) {
    size_t pos = 0;
    while ((pos = msg->find(placeholder, pos)) != std::string::npos) {
        msg->replace(pos, placeholder.length(), value);
        pos += value.length();
    }
}

void CustomMessage::AddLineBreaks(std::string* msg) {
    const float MAX_TEXTBOX_WIDTH = 300.0f;
    const int MAX_LINES_PER_PAGE = 4;

    float currentLineWidth = 0.0f;
    int currentLineCount = 0;
    size_t lastSpaceIndex = std::string::npos;

    for (size_t i = 0; i < msg->size(); ++i) {
        char currentChar = (*msg)[i];

        if (currentChar >= 0x20 && currentChar < 0x20 + sizeof(sNESFontWidths) / sizeof(sNESFontWidths[0])) {
            currentLineWidth += sNESFontWidths[currentChar - 0x20];
        }

        if (currentChar == ' ') {
            lastSpaceIndex = i;
        }
        if (currentLineWidth > MAX_TEXTBOX_WIDTH) {
            if (lastSpaceIndex != std::string::npos) {
                (*msg)[lastSpaceIndex] = 0x11;
                i = lastSpaceIndex;
            } else {
                msg->insert(i, 1, 0x11);
            }

            currentLineWidth = 0.0f;
            lastSpaceIndex = std::string::npos;
            ++currentLineCount;

            if (currentLineCount >= MAX_LINES_PER_PAGE) {
                msg->insert(i + 1, 1, 0x10);
                ++i;
                currentLineCount = 0;
            }
        }
    }
}

// Ensure that the message ends with the message end character
void CustomMessage::EnsureMessageEnd(std::string* msg) {
    if (msg->back() != 0xBF) {
        msg->push_back(0xBF);
    }
}

CustomMessage::Entry CustomMessage::LoadVanillaMessageTableEntry(u16 textId) {
    MessageContext* msgCtx = &gPlayState->msgCtx;
    MessageTableEntry* msgEntry = msgCtx->messageEntryTableNes;
    while (msgEntry->textId != 0xFFFF) {
        if (msgEntry->textId == textId) {
            break;
        }
        msgEntry++;
    }

    CustomMessage::Entry entry;

    entry.textboxType = msgEntry->segment[0];
    entry.textboxYPos = msgEntry->segment[1];
    entry.icon = msgEntry->segment[2];
    entry.nextMessageID = (msgEntry->segment[3] << 8) | msgEntry->segment[4];
    entry.firstItemCost = (msgEntry->segment[5] << 8) | msgEntry->segment[6];
    entry.secondItemCost = (msgEntry->segment[7] << 8) | msgEntry->segment[8];
    entry.msg = std::string(msgEntry->segment + MESSAGE_HEADER_SIZE, msgEntry->msgSize - MESSAGE_HEADER_SIZE);

    return entry;
}

void CustomMessage::LoadCustomMessageIntoFont(CustomMessage::Entry entry) {
    MessageContext* msgCtx = &gPlayState->msgCtx;
    Font* font = &msgCtx->font;

    char buff[1280] = { 0 };

    // Copy message header
    buff[0] = entry.textboxType;
    buff[1] = entry.textboxYPos;
    buff[2] = entry.icon;
    buff[3] = (entry.nextMessageID & 0xFF00) >> 8;
    buff[4] = (entry.nextMessageID & 0x00FF);
    buff[5] = (entry.firstItemCost & 0xFF00) >> 8;
    buff[6] = (entry.firstItemCost & 0x00FF);
    buff[7] = (entry.secondItemCost & 0xFF00) >> 8;
    buff[8] = (entry.secondItemCost & 0x00FF);
    buff[9] = 0xFF;
    buff[10] = 0xFF;

    if (entry.autoFormat) {
        CustomMessage::AddLineBreaks(&entry.msg);
        CustomMessage::EnsureMessageEnd(&entry.msg);
        CustomMessage::Replace(&entry.msg, "\n", "\x11");
    }

    // If message is too long, truncate it and add the message end character
    if (entry.msg.length() > BUFFER_SIZE - MESSAGE_HEADER_SIZE) {
        entry.msg = entry.msg.substr(0, BUFFER_SIZE - MESSAGE_HEADER_SIZE - 1);
        entry.msg += '\xBF';
    }

    memcpy(buff + MESSAGE_HEADER_SIZE, entry.msg.c_str(), entry.msg.length());

    msgCtx->msgLength = entry.msg.length() + MESSAGE_HEADER_SIZE;
    memcpy(&font->msgBuf, buff, msgCtx->msgLength);
}

void CustomMessage::RegisterHooks() {
    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnOpenText>(
        CUSTOM_MESSAGE_ID, [](u16* textId, bool* loadFromMessageTable) {
            *loadFromMessageTable = false;
            CustomMessage::LoadCustomMessageIntoFont(activeCustomMessage);
        });
}
