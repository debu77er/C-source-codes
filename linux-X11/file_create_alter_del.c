#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

// Function prototypes
void create_file(Display *display, Window window);
void open_file(Display *display, Window window);
void delete_file(Display *display, Window window);
void display_message(Display *display, Window window, const char *msg);
void create_button(Display *display, Window parent, int x, int y, int width, int height, const char *label, void (*callback)(Display*, Window));

typedef struct {
    Display *display;
    Window window;
} AppContext;

// Global variables for buttons
Window createBtn, openBtn, deleteBtn;
Display *dpy;
Window mainWin;
XEvent event;

// Forward declarations of callback functions
void createFileCallback(Display *display, Window window);
void openFileCallback(Display *display, Window window);
void deleteFileCallback(Display *display, Window window);

int main() {
    dpy = XOpenDisplay(NULL);
    if (dpy == NULL) {
        fprintf(stderr, "Cannot open display");
        return 1;
    }

    int screen = DefaultScreen(dpy);
    unsigned long black = BlackPixel(dpy, screen);
    unsigned long white = WhitePixel(dpy, screen);

    // Create main window
    mainWin = XCreateSimpleWindow(dpy, RootWindow(dpy, screen), 10, 10, 400, 300, 1, black, white);
    XSelectInput(dpy, mainWin, ExposureMask | KeyPressMask | ButtonPressMask);
    XMapWindow(dpy, mainWin);

    // Create buttons
    createBtn = XCreateSimpleWindow(dpy, mainWin, 20, 20, 100, 30, 1, black, white);
    openBtn = XCreateSimpleWindow(dpy, mainWin, 140, 20, 100, 30, 1, black, white);
    deleteBtn = XCreateSimpleWindow(dpy, mainWin, 260, 20, 100, 30, 1, black, white);

    // Map buttons
    XMapWindow(dpy, createBtn);
    XMapWindow(dpy, openBtn);
    XMapWindow(dpy, deleteBtn);

    // Set window names (labels) using XStoreName
    // For simplicity, we'll draw text on buttons in the Expose event

    // Event loop
    while (1) {
        XNextEvent(dpy, &event);
        if (event.type == Expose) {
            // Draw buttons labels
            GC gc = XCreateGC(dpy, mainWin, 0, NULL);
            XSetForeground(dpy, gc, BlackPixel(dpy, DefaultScreen(dpy)));
            // Draw labels for buttons
            XDrawString(dpy, createBtn, gc, 10, 20, "Create", strlen("Create"));
            XDrawString(dpy, openBtn, gc, 20, 20, "Open", strlen("Open"));
            XDrawString(dpy, deleteBtn, gc, 10, 20, "Delete", strlen("Delete"));
            XFreeGC(dpy, gc);
        } else if (event.type == ButtonPress) {
            // Check which button was pressed
            if (event.xbutton.window == createBtn) {
                createFile(dpy, mainWin);
            } else if (event.xbutton.window == openBtn) {
                open_file(dpy, mainWin);
            } else if (event.xbutton.window == deleteBtn) {
                delete_file(dpy, mainWin);
            }
        } else if (event.type == KeyPress) {
            // Exit on Escape key
            break;
        }
    }

    XCloseDisplay(dpy);
    return 0;
}

void create_file(Display *display, Window window) {
    int fd = open("testfile.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) {
        const char *msg = "This is a new file.\n";
        write(fd, msg, strlen(msg));
        close(fd);
        display_message(display, window, "File created: testfile.txt");
    } else {
        display_message(display, window, "Failed to create file");
    }
}

void open_file(Display *display, Window window) {
    int fd = open("testfile.txt", O_RDONLY);
    if (fd >= 0) {
        char buffer[256];
        ssize_t n = read(fd, buffer, sizeof(buffer) - 1);
        close(fd);
        if (n > 0) {
            buffer[n] = '\0';
            display_message(display, window, "File contents:");
            display_message(display, window, buffer);
        } else {
            display_message(display, window, "Failed to read file");
        }
    } else {
        display_message(display, window, "File not found");
    }
}

void delete_file(Display *display, Window window) {
    if (remove("testfile.txt") == 0) {
        display_message(display, window, "File deleted: testfile.txt");
    } else {
        display_message(display, window, "Failed to delete file");
    }
}

// Function to display message (simple implementation)
void display_message(Display *display, Window window, const char *msg) {
    // For simplicity, just print to terminal
    printf("%s", msg);
    // Alternatively, could draw on the window
}

// Note: For more advanced GUI, you'd need to handle text rendering, scrolling, etc.
// This example is minimal and intended to show basic interaction.
