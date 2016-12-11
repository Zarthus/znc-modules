/*
 * Copyright (C) 2004-2014 ZNC, see the NOTICE file for details.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Originally, this module is just a modified http://wiki.znc.in/Highlightattach,
 * But this module had a few issues, namely that you can get joined to a channel midway of conversing.
 * The way this module is built, it just notifies you of it, so you can manually join that channel.
 *
 * This module was made by Zarthus <zarthus@zarth.us> for personal use, and is not officially endorsed by ZNC in any way.
 * Since 2014-08-24
 */


#include <znc/IRCNetwork.h>
#include <znc/Modules.h>
#include <znc/Chan.h>

class CHighlightDetachNotifyMod : public CModule {
public:
    MODCONSTRUCTOR(CHighlightDetachNotifyMod) {
        AddHelpCommand();
        AddCommand("getname", static_cast<CModCommand::ModCmdFunc>(&CHighlightDetachNotifyMod::GetName), "", "Gets highlight name set by 'setname'.");
        AddCommand("setname", static_cast<CModCommand::ModCmdFunc>(&CHighlightDetachNotifyMod::SetName), "<name>", "Sets highlight name to <name>.");

        if (GetNV("highlight_name").empty()) {
            SetNV("highlight_name", m_pNetwork->GetCurNick());
        }
    }
    ~CHighlightDetachNotifyMod() {}

    EModRet OnChanMsg(CNick& Nick, CChan& Channel, CString& sMessage) {
        if (Channel.IsDetached()) {
            if (sMessage.AsLower().find(GetNV("highlight_name").AsLower()) != CString::npos) {
                PutModule(Nick.GetNick() + " mentioned your name in " + Channel.GetName() + " saying: " + sMessage);
            }
        }
        return CONTINUE;
    }

    void GetName(const CString& sMessage) {
        if (GetNV("highlight_name").empty()) {
            PutModule("You do not have any name set. Set one using 'setname'.");
        } else {
            PutModule("Name is set to " + GetNV("highlight_name") + ".");
        }
    }

    void SetName(const CString& sName) {
        if (GetNV("highlight_name").empty()) {
            SetNV("highlight_name", sName.Token(1));
            PutModule("Name has been set to: " + sName.Token(1));
        } else if (GetNV("highlight_name").Equals(sName.Token(1))) {
            PutModule("Name was already set to " + sName.Token(1) + ".");
        } else {
            PutModule("Name was already set to " + GetNV("highlight_name") + ", that name has been unset and your new highlight name is on " + sName.Token(1) + ".");
            DelNV("highlight_name");
            SetNV("highlight_name", sName.Token(1));
        }
    }
};

template<> void TModInfo<CHighlightDetachNotifyMod>(CModInfo& Info) {
    Info.SetWikiPage("highlight_detach_notify");
    Info.SetHasArgs(false);
    Info.SetArgsHelpText("Send a message when you get highlighted in a detached channel.");
}

MODULEDEFS(CHighlightDetachNotifyMod, "Send a message when you get highlighted in a detached channel.")
