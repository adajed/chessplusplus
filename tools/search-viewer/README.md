# Search Viewer
Development tool for investigating search debug logs.

## Requirements 

### python
```
python-chess
curses
sqlite3
```
### other
chessplusplus built with `LOG_LEVEL=2`

## Example usage
```
./build/chessplusplus 2>./tools/search-viewer/debug.log
< uci
< position fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
< go depth 7
# wait for search to end...
< quit
cd tools/search-viewer/
python3 create_db.py -l debug.log -o debug.sqlite # this might take few minutes
python3 search_viewer.py debug.sqlite
```
