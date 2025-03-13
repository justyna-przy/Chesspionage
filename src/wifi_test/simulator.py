import time
import requests
import chess

# Update this URL to match your Flask server address (e.g., http://localhost:5000/log)
SERVER_URL = "http://localhost:5000/log"

# PGN moves (from chess.com, which is known to be legal)
# pgn = (
#     "1. e4 e5 2. Nf3 Nc6 3. d4 Qf6 4. d5 Nd4 5. c3 Nxf3+ 6. Qxf3 Be7 7. Qxf6 Bxf6 "
#     "8. Nd2 d6 9. Nf3 c6 10. c4 cxd5 11. cxd5 b6 12. Bb5+ Bd7 13. Bxd7+ Kxd7 14. O-O Ne7 "
#     "15. Bg5 Bxg5 16. Nxg5 f6 17. Nf3 Rac8 18. Rac1 g5 19. Rxc8 Rxc8 20. b3 g4 21. Nd2 Rc2 "
#     "22. f3 Rxd2 23. fxg4 Rxa2 24. Rxf6 Ra1+ 25. Kf2 Ra2+ 26. Kg3 Ra3 27. g5 Rxb3+ 28. Kg4 a5 "
#     "29. Rf7 a4 30. Rxh7 a3 31. h4 a2 32. h5 a1=Q 33. g6 Qd1+ 34. Kg5 Rg3+ 35. Kf6 Qf1+ "
#     "36. Kg7 Ke8 37. Kh6 Qf8+ 38. Rg7 Qh8+ 39. Rh7 Qf8+ 40. Rg7 Nxg6 41. hxg6 Rxg2 "
#     "42. Kh7 Rh2# 0-1"
# )

pgn = "1. e4 e5 2. Nf3 Nc6 3. Bc4 Nf6 4. Ng5 d5 5. exd5 Nxd5 6. Nxf7 Kxf7 7. Qf3+ Ke8 8. Bxd5 Bd6 9. Qf7# 1-0"

def extract_moves(pgn_string):
    tokens = pgn_string.split()
    moves_san = []
    for token in tokens:
        # Skip move numbers (ending with a dot) and game result indicators
        if token.endswith('.') or token in ["0-1", "1-0", "1/2-1/2"]:
            continue
        moves_san.append(token)
    return moves_san

def simulate_moves():
    moves_san = extract_moves(pgn)
    board = chess.Board()
    
    for move_san in moves_san:
        # If it's black's turn and the move involves a promotion,
        # convert the promotion piece letter to lowercase (e.g., "a1=Q" -> "a1=q").
        if not board.turn and "=" in move_san:
            for piece in ["Q", "R", "B", "N"]:
                move_san = move_san.replace("="+piece, "="+piece.lower())
        
        try:
            # Convert SAN move to a Move object
            move = board.parse_san(move_san)
        except Exception as e:
            print(f"Error parsing move '{move_san}': {e}")
            continue

        # Get UCI representation (e.g., 'e2e4' or 'a7a8q' for promotions)
        uci_move = move.uci()
        # Split into source and destination
        move_data = {"from": uci_move[0:2], "to": uci_move[2:4]}
        # Include promotion if present (e.g., a7a8q)
        if len(uci_move) == 5:
            move_data["promotion"] = uci_move[4]
        
        print(f"Posting move: {move_data}")

        try:
            response = requests.post(SERVER_URL, json=move_data)
            print("Server response:", response.json())
        except Exception as e:
            print(f"Error posting move {move_data}: {e}")
        
        # Update the board state with the move
        board.push(move)
        # Wait for 1 second before sending the next move (adjust as needed)
        time.sleep(0.5)

if __name__ == '__main__':
    simulate_moves()