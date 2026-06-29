#!/bin/sh

db_file=server/db.sqlite3

sqlite3 "$db_file" <<EOF

CREATE TABLE IF NOT EXISTS
alerts
(
    id          INTEGER PRIMARY KEY AUTOINCREMENT,
    item_id     INTEGER REFERENCES items(id),
    status      TEXT DEFAULT '',
    battery     INTEGER DEFAULT 0,
    boot_count  INTEGER DEFAULT 0,
    rep_wakeups INTEGER DEFAULT 0,
    bssid       TEXT DEFAULT '',
    datetime    DATETIME DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS
items
(
    id          INTEGER PRIMARY KEY,
    room        TEXT DEFAULT '',
    description TEXT DEFAULT ''
);

CREATE TABLE IF NOT EXISTS
next_wakeups
(
    item_id  INTEGER PRIMARY KEY REFERENCES items(id),
    datetime DATETIME
);

CREATE TABLE IF NOT EXISTS
access_points
(
    bssid       TEXT PRIMARY KEY,
    description TEXT DEFAULT ''
);

EOF
