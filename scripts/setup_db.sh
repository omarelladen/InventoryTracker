#!/bin/sh

db_file=server/db.sqlite3

sqlite3 "$db_file" <<EOF

CREATE TABLE IF NOT EXISTS
alerts
(
    id         INTEGER PRIMARY KEY AUTOINCREMENT,
    item_id    INTEGER REFERENCES items(id),
    status     TEXT DEFAULT '',
    battery    INTEGER DEFAULT 0,
    boot_count INTEGER DEFAULT 0,
    datetime   DATETIME DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS
items
(
    id          INTEGER PRIMARY KEY,
    room        TEXT DEFAULT '',
    description TEXT DEFAULT ''
);

EOF
