import os

import sqlite3
import uvicorn
from fastapi import FastAPI, Form, HTTPException
from fastapi.responses import HTMLResponse, Response, RedirectResponse
from pydantic import BaseModel


db_path = "db.sqlite3"

app = FastAPI()


class Alert(BaseModel):
    item_id:    int
    status:     str
    battery:    int
    boot_count: int

class Item(BaseModel):
    id:          int
    room:        str
    description: str


def create_html_table(query):
    with sqlite3.connect(db_path, timeout=60) as con:
        cur = con.cursor()
        cur.execute(query)

        list_items = cur.fetchall()
        list_headers = [d[0] for d in cur.description]

    list_html_lines = []
    for item in list_items:
        cells = "".join([f"<td>{v}</td>" for v in item])
        list_html_lines.append(f"<tr>{cells}</tr>")

    list_html_headers = []
    for i in list_headers:
        list_html_headers.append(f"<th>{i}</th>")

    html_table = f"""
    <table border="1" style="border-collapse: collapse; width: 100%;">
        <thead>
            <tr>
                {"".join(list_html_headers)}
            </tr>
        </thead>
        <tbody>
            {"".join(list_html_lines)}
        </tbody>
    </table>
    """

    return html_table


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

@app.get("/register", response_class=HTMLResponse)
async def get_register():
    with open("register.html", "r", encoding="utf-8") as f:
        content = f.read()
    return HTMLResponse(content=content)

@app.get("/update", response_class=HTMLResponse)
async def get_update():
    with open("update.html", "r", encoding="utf-8") as f:
        content = f.read()
    return HTMLResponse(content=content)

@app.get("/alerts", response_class=HTMLResponse)
async def get_look():
    query = "SELECT * FROM alerts"
    html_table = create_html_table(query)
    return HTMLResponse(
        content=f"""
        <html>
            <head>
                <title>Alertas</title>
            </head>
            <body>
                <h1>Alertas</h1>
                {html_table}
            </body>
        </html>
        """
    )

@app.get("/items", response_class=HTMLResponse)
async def get_items():
    query = "SELECT * FROM items"
    html_table = create_html_table(query)
    return HTMLResponse(
        content=f"""
        <html>
            <head>
                <title>Itens</title>
            </head>
            <body>
                <h1>Itens</h1>
                {html_table}
            </body>
        </html>
        """
    )


@app.post("/alert")
async def alert(alert: Alert):
    print(alert)

    with sqlite3.connect(db_path, timeout=60) as con:
        cur = con.cursor()

        cur.execute("""
            INSERT INTO alerts
            (item_id,
             status,
             battery,
             boot_count)
            VALUES (?, ?, ?, ?)
            """,
            (alert.item_id,
             alert.status,
             alert.battery,
             alert.boot_count)
        )

        con.commit()

    return alert

@app.post("/register")
async def register(
    id:          int = Form(...),
    room:        str = Form(...),
    description: str = Form(...)
):
    item: Item = Item(
        id=id,
        room=room,
        description=description
    )

    with sqlite3.connect(db_path, timeout=60) as con:
        cur = con.cursor()

        try:
            cur.execute("""
                INSERT INTO items
                (id,
                 room,
                 description)
                VALUES (?, ?, ?)
                """,
                (item.id,
                 item.room,
                 item.description)
            )
        except sqlite3.IntegrityError:
            # TODO: show error in page
            raise HTTPException(
                detail="ID already exists",
                status_code=422
            )

        con.commit()

    # TODO: show success msg
    return RedirectResponse(url="/", status_code=303)

@app.post("/update")
async def update(
    id:          int = Form(...),
    room:        str = Form(...),
    description: str = Form(...)
):
    if room == "":
        raise HTTPException(
            detail="Room is empty",
            status_code=422
        )

    with sqlite3.connect(db_path) as con:
        cur = con.cursor()
        cur.execute("""
            UPDATE items
            SET room = ?,
                description = ?
            WHERE id = ?
            """,
            (room,
             description,
             id)
        )
        if cur.rowcount == 0:
            raise HTTPException(
                detail="Item not found",
                status_code=404
            )

    return RedirectResponse(url="/", status_code=303)


if __name__ == "__main__":
    uvicorn.run(
        app="main:app",
        host="0.0.0.0",  # open
        port=8000,
        reload=True,
    )
