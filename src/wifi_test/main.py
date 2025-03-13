from flask import Flask, request, jsonify, render_template_string, url_for
import chess
import chess.engine

app = Flask(__name__)
game_board = chess.Board()  # Global chess board for the current game
pgn_moves = []  # Global list to record SAN moves

# Initialize Stockfish engine using a relative path within the repo
engine = chess.engine.SimpleEngine.popen_uci("stockfish/stockfish-windows-x86-64-avx2.exe")

def get_board_evaluation(board):
    """Analyze the board and return an evaluation string and a percentage (0-100) for the eval bar."""
    analysis = engine.analyse(board, chess.engine.Limit(time=0.1))
    score_obj = analysis.get("score")
    if score_obj is None:
        return "0 cp", 50  # neutral

    if score_obj.is_mate():
        mate_in = score_obj.white().mate()
        score_str = f"Mate in {mate_in}" if mate_in > 0 else f"Mate in {abs(mate_in)}"
        eval_percent = 100 if mate_in > 0 else 0
    else:
        score = score_obj.white().score(mate_score=1000)
        score_str = f"{score} cp"
        # Normalize from -1000 cp (0%) to +1000 cp (100%), with 0 cp at 50%
        eval_percent = (score + 1000) / 2000 * 100
        eval_percent = max(0, min(100, eval_percent))
    return score_str, eval_percent

def get_board_state():
    board_fen = game_board.fen()
    score_str, eval_percent = get_board_evaluation(game_board)
    state = {
        "fen": board_fen,
        "score_str": score_str,
        "eval_percent": eval_percent,
        "pgn": " ".join(pgn_moves)
    }
    return state

@app.route('/')
def index():
    state = get_board_state()
    return render_template_string('''
<!doctype html>
<html>
  <head>
    <title>Live Chess Game State</title>
    <!-- Load jQuery from a reliable CDN -->
    <script src="https://cdnjs.cloudflare.com/ajax/libs/jquery/3.6.0/jquery.min.js"></script>
    <!-- Load Chessboard.js CSS and JS from your local static folder -->
    <link rel="stylesheet" href="{{ url_for('static', filename='chessboardjs-1.0.0/css/chessboard-1.0.0.css') }}">
    <script src="{{ url_for('static', filename='chessboardjs-1.0.0/js/chessboard-1.0.0.js') }}"></script>
    <style>
      body { font-family: Arial, sans-serif; margin: 2em; }
      /* Container for board and right column */
      #container { display: flex; }
      #board-container { width: 600px; }
      #board { width: 600px; height: 600px; }
      /* Right column: now split into two columns */
      #right-container { 
          width: 300px; 
          display: flex; 
          flex-direction: row;
          align-items: flex-start;
      }
      /* Evaluation bar container on the left of the right column */
      #evalBarContainer {
          position: relative;
          width: 40px;
          height: 600px;
          border: 1px solid #000;
      }
      /* White fill (top) and black fill (bottom) */
      #whiteFill {
          position: absolute;
          top: 0;
          width: 100%;
          background-color: white;
          transition: height 0.5s ease;
      }
      #blackFill {
          position: absolute;
          bottom: 0;
          width: 100%;
          background-color: black;
          transition: height 0.5s ease;
      }
      /* Info container to the right of the eval bar */
      #infoContainer { 
          flex: 1; 
          margin-left: 10px;
          display: flex;
          flex-direction: column;
          align-items: stretch;
      }
      #evalText { margin-bottom: 10px; }
      #pgnField {
          flex: 1;
          font-family: monospace;
          margin-bottom: 10px;
      }
      #resetButton {
          display: none;
          font-size: 16px;
          padding: 10px 20px;
      }
    </style>
  </head>
  <body>
    <h1>Live Chess Game State</h1>
    <div id="container">
      <div id="board-container">
        <div id="board"></div>
      </div>
      <div id="right-container">
        <div id="evalBarContainer">
          <div id="whiteFill" style="height: {{ state['eval_percent'] }}%;"></div>
          <div id="blackFill" style="height: {{ 100 - state['eval_percent'] }}%;"></div>
        </div>
        <div id="infoContainer">
          <div id="evalText">Engine Evaluation: {{ state['score_str'] }}</div>
          <textarea id="pgnField" readonly>{{ state['pgn'] }}</textarea>
          <button id="resetButton">Reset Board</button>
        </div>
      </div>
    </div>
    <script>
      // Initialize the chess board with the initial FEN from the server,
      // and specify the pieceTheme to point to your local static folder.
      var board = Chessboard('board', {
          position: '{{ state["fen"] }}',
          pieceTheme: '/static/chessboardjs-1.0.0/img/chesspieces/wikipedia/{piece}.png'
      });
      
      // Function to poll the /state endpoint and update board, evaluation, and PGN.
      function updateState() {
        $.getJSON('/state', function(data) {
          board.position(data.fen);
          var evalText = 'Engine Evaluation: ' + data.score_str;
          if (data.message) {
            evalText += " — " + data.message;
            $('#resetButton').show();
          } else {
            $('#resetButton').hide();
          }
          $('#evalText').text(evalText);
          // Update vertical eval bar: whiteFill height = eval_percent, blackFill = remainder.
          $('#whiteFill').css('height', data.eval_percent + '%');
          $('#blackFill').css('height', (100 - data.eval_percent) + '%');
          $('#pgnField').val(data.pgn);
        });
      }
      
      // Poll every 2 seconds for live updates.
      setInterval(updateState, 2000);
      
      // Reset board when reset button is pressed.
      $('#resetButton').click(function(){
        $.post('/reset', function(data){
          board.position(data.fen);
          $('#evalText').text('Engine Evaluation: ' + data.score_str);
          $('#whiteFill').css('height', data.eval_percent + '%');
          $('#blackFill').css('height', (100 - data.eval_percent) + '%');
          $('#pgnField').val(data.pgn);
          $('#resetButton').hide();
        });
      });
    </script>
  </body>
</html>
    ''', state=get_board_state())

@app.route('/state')
def state():
    state = get_board_state()
    if game_board.is_game_over():
        result = game_board.result()  # "1-0", "0-1", or "1/2-1/2"
        if result == "1-0":
            state["message"] = "White wins"
        elif result == "0-1":
            state["message"] = "Black wins"
        else:
            state["message"] = "Draw"
        state["game_over"] = True
    else:
        state["game_over"] = False
    return jsonify(state)

@app.route('/reset', methods=['POST'])
def reset():
    global game_board, pgn_moves
    game_board = chess.Board()  # Reset the board
    pgn_moves = []            # Clear recorded moves
    return jsonify(get_board_state())

@app.route('/log', methods=['POST'])
def log():
    global game_board, pgn_moves
    data = request.get_json()
    if not data or 'from' not in data or 'to' not in data:
        return jsonify({"status": "fail", "message": "Invalid move data"}), 400

    # Build UCI move string and include promotion if provided.
    move_uci = data.get("from") + data.get("to")
    if "promotion" in data:
        move_uci += data.get("promotion")
    move = chess.Move.from_uci(move_uci)
    
    if move in game_board.legal_moves:
        san = game_board.san(move)  # Get SAN notation before pushing the move.
        game_board.push(move)
        pgn_moves.append(san)
        print("Updated board:", game_board)
        analysis = engine.analyse(game_board, chess.engine.Limit(time=0.1))
        print("Engine evaluation score:", analysis.get("score"))
        return jsonify({"status": "success", "move": move_uci}), 200
    else:
        return jsonify({"status": "fail", "message": "Illegal move"}), 400

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
