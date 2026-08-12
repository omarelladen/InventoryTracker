#!/bin/sh

incus exec debian13-fastapi -- rm -rf server/
incus file push -r server/ debian13-fastapi/root/
incus file push scripts/setup_db.sh debian13-fastapi/root/
