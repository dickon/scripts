#!/usr/bin/python
#
# Copyright (c) 2014 Citrix Systems, Inc.
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

import json
import os
import subprocess
import sys

# Categories for JIRA issues based on resolution and fix version.
(NON_EXISTENT,
 UNRESOLVED_ACTIVE,
 UNRESOLVED_OUTGOING,
 RESOLVED_FIXED,
 RESOLVED_NOT_FIXED) = range(5)

# Represents a change to a JIRA issue.
class Transition(object):
    def __init__(self, data):
        event = data["webhookEvent"]

        if event == "jira:issue_created":
            self.new = State(data["issue"])
            self.old = State(None)
        elif event == "jira:issue_updated":
            self.new = State(data["issue"])
            self.old = State(data["issue"])
            self.old.unapply_changelog(data.get("changelog"))
        else:
            log("unknown event:", event)
            return

        log("old state:", self.old)
        log("new state:", self.new)

    def get_sound(self):
        new_category = self.new.get_category()

        if new_category != self.old.get_category():
            if new_category == UNRESOLVED_ACTIVE:
                return "cdim.wav"
            elif new_category == UNRESOLVED_OUTGOING:
                return "g7-am.wav"
            elif new_category == RESOLVED_FIXED:
                return "g7-c.wav"
            elif new_category == RESOLVED_NOT_FIXED:
                return "gm-cm.wav"
        elif self.new.assignee != self.old.assignee:
            return "c.wav"

# Represents the old or new state of a JIRA issue.
class State(object):
    def __init__(self, issue):
        self.exists = False
        self.fix_versions = set()
        self.resolution = None
        self.assignee = None

        if issue:
            fields = issue["fields"]
            self.exists = True
            self.fix_versions = set(x["name"] for x in fields["fixVersions"])

            if fields["resolution"]:
                self.resolution = fields["resolution"]["name"]

            if fields["assignee"]:
                self.assignee = fields["assignee"]["name"]

    def unapply_changelog(self, changelog):
        if changelog:
            for item in reversed(changelog["items"]):
                if item["field"] == "Fix Version":
                    if item["toString"]:
                        self.fix_versions.discard(item["toString"])
                    if item["fromString"]:
                        self.fix_versions.add(item["fromString"])
                elif item["field"] == "resolution":
                    self.resolution = item["fromString"]
                elif item["field"] == "assignee":
                    self.assignee = item["from"]

    def get_category(self):
        if not self.exists:
            return NON_EXISTENT

        if self.resolution is None:
            for version in self.fix_versions:
                if "outgoing" not in version.lower():
                    return UNRESOLVED_ACTIVE

            return UNRESOLVED_OUTGOING

        if self.resolution == "Fixed":
            return RESOLVED_FIXED

        return RESOLVED_NOT_FIXED

    def __str__(self):
        return ("exists={0!r}, "
                "fix_versions={1!r}, "
                "resolution={2!r}, "
                "assignee={3!r}, "
                "category={4!r}".format(self.exists,
                                        self.fix_versions,
                                        self.resolution,
                                        self.assignee,
                                        self.get_category()))

def main():
    print "Content-type: text/plain\n"

    data = json.load(sys.stdin)
    log("input:\n" + json.dumps(data, indent=4, sort_keys=True))

    transition = Transition(data)
    sound = transition.get_sound()

    if sound:
        play(sound)

def play(sound):
    log("play:", sound)
    wav = os.path.join(os.path.dirname(sys.argv[0]), "..", "sounds", sound)

    subprocess.check_call(["amixer", "-q", "sset", "Master", "100%", "on"])
    subprocess.check_call(["amixer", "-q", "sset", "PCM", "85%", "on"])
    subprocess.check_call(["aplay", "-q", wav])

def log(*args):
    sys.stderr.write(" ".join([str(x) for x in args]) + "\n")

if __name__ == "__main__":
    main()
