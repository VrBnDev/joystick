# Importa as bibliotecas Flask e SocketIO
from flask import Flask, render_template, request
from flask_socketio import SocketIO, emit

# Cria a instância do Flask
app = Flask(__name__)

# Configura o SocketIO para permitir conexões de qualquer origem
socketio = SocketIO(app, cors_allowed_origins="*")

# Rota principal que serve a página HTML
@app.route('/angulo')
def index():
    angulo = request.args.get('valor')
    print(f"Ângulo recebido: {angulo}")
    socketio.emit('command', {'angulo': angulo})
    return render_template('index.html')  # Renderiza o template web/templates/index.html

# Ponto de entrada principal da aplicação
if __name__ == '__main__':
    # Inicia o servidor Flask com suporte a WebSockets
    socketio.run(app, host='0.0.0.0', port=5000)
