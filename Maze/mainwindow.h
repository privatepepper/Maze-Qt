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

private slots:
    void on_pushButton_clicked();
    void MySlot();
    void solve_slot();


    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_actiontest_triggered();

    void on_actionDark_triggered();

private:
    Ui::MainWindow *ui;

    QGraphicsScene *scene;

    QTimer *timer;
    QTimer *timer1;

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

    QPen wall_pen;
    QBrush maze_background_color;
    QBrush start_and_path_background_color;
    QBrush last_square_background_color;
    QBrush window_background_color;
    QBrush head;

    bool unvisited_squares();

    QVector < QGraphicsLineItem* > check_neighbours(int y, int x);
    QVector < QGraphicsLineItem* > legal_move(int y, int x);

    QVector < QGraphicsLineItem* > next_square;
    QVector < QGraphicsLineItem* > current_square;

    QVector < QPair <int, int> > memorize;
    QVector < QPair <int, int> > memorize_solve_function;


    QVector < QVector < QGraphicsRectItem* > > background_color_squares;
    void reset_background_color();

    QVector < QVector < int > > visited_walls;
    void reset_visited_walls();

    QVector < QVector < QVector <QGraphicsLineItem* > > > walls;
    void reset_walls();

    void instant_maze_generation();
    void instant_solve();
};
#endif // MAINWINDOW_H
