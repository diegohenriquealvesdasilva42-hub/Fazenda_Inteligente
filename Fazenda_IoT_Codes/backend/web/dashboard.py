from flask import Flask, render_template
import mysql.connector
import os
from dotenv import load_dotenv

load_dotenv()

app = Flask(__name__)

def get_conexao():
    """
    Cria e retorna uma nova conexão com o banco de dados MySQL
    usando variáveis de ambiente (.env)
    """
    return mysql.connector.connect(
        host=os.getenv("DB_HOST"),
        database=os.getenv("DB_NAME"),
        user=os.getenv("DB_USER"),
        password=os.getenv("DB_PASS")
    )

@app.route("/")
def index():
    con = get_conexao()
    cursor = con.cursor(dictionary=True)

    cursor.execute("""
        SELECT *
        FROM sensores
        ORDER BY data_hora DESC
        LIMIT 1
    """)
    dado = cursor.fetchone()

    cursor.close()
    con.close()

    return render_template("index.html", dado=dado)

@app.route("/historico")
def historico():
    con = get_conexao()
    cursor = con.cursor(dictionary=True)

    cursor.execute("""
        SELECT *
        FROM sensores
        ORDER BY data_hora DESC
        LIMIT 30
    """)
    dados = cursor.fetchall()

    cursor.close()
    con.close()

    return render_template("historico.html", dados=dados)

if __name__ == "__main__":
    app.run(debug=True)
