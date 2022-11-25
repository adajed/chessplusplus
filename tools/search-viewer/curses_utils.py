import curses

def get_centered_position(length, max_size):
    assert(length <= max_size)
    return int((max_size - length) / 2)

def print_centered(window, line, text):
    (height, width) = window.getmaxyx()
    start_x = get_centered_position(len(text), width)
    window.addstr(line, start_x, text)

def window_split_horizontally(window, split=0.5):
    (y, x) = window.getmaxyx()

    if isinstance(split, int):
        assert(split > 0 and split < y)
        y_split = split
    elif isinstance(split, float):
        assert(split > 0 and split < 1)
        y_split = int(y * split)
    else:
        raise Exception("split must be int or float")

    top_window = window.derwin(y_split, x, 0, 0)
    bottom_window = window.derwin(y - y_split, x, y_split, 0)
    return top_window, bottom_window

def window_split_vertically(window, split=0.5):
    (y, x) = window.getmaxyx()

    if isinstance(split, int):
        assert(split > 0 and split < x)
        x_split = split
    elif isinstance(split, float):
        assert(split > 0 and split < 1)
        x_split = int(x * split)
    else:
        raise Exception("split must be int or float")

    left_window = window.derwin(y, x_split, 0, 0)
    right_window = window.derwin(y, x - x_split, 0, x_split)
    return left_window, right_window
