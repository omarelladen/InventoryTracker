#!/bin/sh

incus config device remove debian13-fastapi proxy-fastapi
incus config device add debian13-fastapi proxy-fastapi proxy listen=tcp:0.0.0.0:8000 connect=tcp:10.210.67.50:8000
