import os

import sqlite3

import uvicorn
from fastapi import FastAPI, Form
from fastapi.responses import HTMLResponse, Response, RedirectResponse
from pydantic import BaseModel


class Alert(BaseModel):
    board_id:   int
    status:     str
    battery:    int
    boot_count: int

class Item(BaseModel):
    board_id:    int
    room:        str
    description: str


app = FastAPI()


@app.get("/favicon.ico", include_in_schema=False)
async def favicon():
    return Response(status_code=204)  # FileResponse("favicon.ico")

@app.get("/")
async def root():
    return await get_index()

@app.get("/index.html", response_class=HTMLResponse)
async def get_index():
    with open("index.html", "r", encoding="utf-8") as f:
        content = f.read()
    return HTMLResponse(content=content)

@app.get("/register.html", response_class=HTMLResponse)
async def get_register():
    with open("register.html", "r", encoding="utf-8") as f:
        content = f.read()
    return HTMLResponse(content=content)


@app.post("/alert")
async def alert(alert: Alert):
    print(alert)

    with sqlite3.connect("db.sqlite3", timeout=60) as con:
        cur = con.cursor()

        cur.execute("""
            INSERT INTO alerts
            (board_id,
             status,
             battery,
             boot_count)
            VALUES (?, ?, ?, ?)
            """,
            (alert.board_id,
             alert.status,
             alert.battery,
             alert.boot_count)
        )

        con.commit()

    return alert

@app.post("/register")
async def register(
    board_id:    int = Form(...),
    room:        str = Form(...),
    description: str = Form(...)
):
    item: Item = Item(
        board_id=board_id,
        room=room,
        description=description
    )
    print(item)

    with sqlite3.connect("db.sqlite3", timeout=60) as con:
        cur = con.cursor()

        try:
            cur.execute("""
                INSERT INTO items
                (board_id,
                 room,
                 description)
                VALUES (?, ?, ?)
                """,
                (item.board_id,
                 item.room,
                 item.description)
            )
        except sqlite3.IntegrityError:
            print("Error: ID already exists")
            # TODO: add error response

        con.commit()

    # TODO: show success msg
    return RedirectResponse(url="/", status_code=303)


if __name__ == "__main__":
    uvicorn.run(
        app="main:app",
        host="0.0.0.0",  # open
        port=8000,
        reload=True,
    )
