import curses
from curses import panel

# clips x to be between min_value and max_value
def clip(x, min_value, max_value):
    return min(max(x, min_value), max_value)

def get_centered_position(length, max_size):
    assert(length <= max_size)
    return int((max_size - length) / 2)


class MenuItem:
    def __init__(self, name, panel):
        self.name = name
        self.panel = panel


class ExitPanel:
    def __init__(self):
        pass

    def preview(self, window):
        window.clear()


class Menu:
    def __init__(self, items, screen, use_index=True):
        self.main_window = screen.derwin(0, 0)
        self.main_window.keypad(1)

        (height, width) = self.main_window.getmaxyx()

        self.tab_window = self.main_window.derwin(height, width // 3, 0, 0)
        self.tab_window.border()
        self.tab_window = self.tab_window.derwin(self.tab_window.getmaxyx()[0] - 2, self.tab_window.getmaxyx()[1] - 2, 1, 1)
        self.preview_window = self.main_window.derwin(height, 2 * (width // 3), 0, width // 3)

        self.panel = panel.new_panel(self.main_window)
        self.panel.hide()
        panel.update_panels()

        self.position = 0
        self.items = items
        self.items.append(MenuItem("Exit", ExitPanel()))

        self.use_index = use_index

    def move(self, n):
        self.position += n
        self.position = clip(self.position, 0, len(self.items) - 1)
        self.items[self.position].panel.preview(self.preview_window)
        self.refresh()

    def refresh(self):
        self.main_window.refresh()
        curses.doupdate()

    def display(self):
        self.panel.top()
        self.panel.show()

        max_y, max_x = self.tab_window.getmaxyx()

        while True:
            self.tab_window.clear()
            start_index = max(0, self.position - max_y // 2)
            end_index = min(len(self.items), start_index + max_y)
            for index in range(start_index, end_index):
                i = index - start_index
                item = self.items[index]
                if index == self.position:
                    mode = curses.A_REVERSE
                else:
                    mode = curses.A_NORMAL

                if self.use_index:
                    msg = f"{index}. {item.name}"
                else:
                    msg = item.name
                start_pos = get_centered_position(len(msg), max_x)
                self.tab_window.addstr(i, start_pos, msg, mode)

            self.move(0)

            self.refresh()
            key = self.main_window.getch()

            if key in [curses.KEY_ENTER, curses.KEY_RIGHT, ord("\n"), ord('l'), ord('L')]:
                if self.position == len(self.items) - 1:
                    break
                else:
                    self.items[self.position].panel.display()

            elif key in [curses.KEY_UP, ord('k'), ord('K')]:
                self.move(-1)

            elif key in [curses.KEY_DOWN, ord('j'), ord('J')]:
                self.move(1)

            elif key in [curses.KEY_BACKSPACE, curses.KEY_LEFT, ord('h'), ord('H'), ord('p'), ord('P')]:
                break

            elif key in [ord('u'), ord('U')]:
                self.move(-self.position)

            elif key in [ord('d'), ord('D')]:
                self.move(len(self.items) - self.position)


        self.main_window.clear()
        self.panel.hide()
        panel.update_panels()
        curses.doupdate()
