#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QtWidgets>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


protected:
    void keyPressEvent(QKeyEvent *event);

private slots:

    void on_pushButton_clicked();

    void MySlot();

    void solve_slot();

    void follower();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_actiontest_triggered();

    void on_actionDark_triggered();

    void on_radioButton_toggled(bool checked);

private:

    Ui::MainWindow *ui;

    QGraphicsScene *scene;

    // Generator timer
    QTimer *timer;
    // Solve timer
    QTimer *timer1;
    // follower timer
    QTimer *timer2;

    int maze_width;
    int maze_height;
    int square_size;
    int next_square_y;
    int next_square_x;
    int current_square_y;
    int current_square_x;
    int memorize_y;
    int memorize_x;
    int count = 1;
    int square_moved = 0;
    int element_pos = 0;

    int timer_is_on = 0;
    int timer1_is_on = 0;

    QPen wall_pen;
    QBrush maze_background_color;
    QBrush start_and_path_background_color;
    QBrush last_square_background_color;
    QBrush window_background_color;
    QBrush head;

    // if maze is generated - true, otherwise false.
    bool unvisited_squares();

    // returns square which hasn't been visited yet
    QVector < QGraphicsLineItem* > check_neighbours(int y, int x);

    QVector < QGraphicsLineItem* > legal_move(int y, int x);

    // if users move with green square is legal - true , else false
    bool is_legal_move(int y, int x, int y1, int x1);

    // use - when you move to the new location, repaint your previous square.
    QPair <int, int> previous_square_cord;

    // if you reach lower right corner, you win the game
    bool won_game();

    // vector that has curret || next square, which represents 4 walls
    QVector < QGraphicsLineItem* > next_square;
    QVector < QGraphicsLineItem* > current_square;

    // using for backtracking
    QVector < QPair <int, int> > memorize;
    QVector < QPair <int, int> > memorize_solve_function;

    // background colors, using to change colors of paths.
    QVector < QVector < QGraphicsRectItem* > > background_color_squares;
    void reset_background_color();

    // stores already visited walls || resets visited walls to 0
    QVector < QVector < int > > visited_walls;
    void reset_visited_walls();

    // 3d vector that stores coordinates of "squares" which are a vector that has QGraphicsLineItems inside, which represent walls
    QVector < QVector < QVector <QGraphicsLineItem* > > > walls;
    void reset_walls();

    // Graph implementation

    QList <int> *my_list;
    int vertices = 0;
    int count_walls(int y, int x);
    void graph_maker();

    // path finder
    QPair <int, int>  path_finder(int start_y, int start_x, int end_y, int end_x);

    QVector <QPair <int, int> > my_path;

    int current_vertice;
    int next_vertice;

    QVector < int > visited_vertices;
    int random_neighbour_vertice(int pos);

    int current_red_y;
    int current_red_x;
    int next_red_y;
    int next_red_x;

   // repaint instantly maze || solve
    void instant_maze_generation();
    void instant_solve();
};
#endif // MAINWINDOW_H
