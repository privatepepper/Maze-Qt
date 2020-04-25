#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFrame>
#include <QLineEdit>
#include <QIntValidator>
#include <QMessageBox>
#include <QPushButton>
#include <QRandomGenerator>
#include <QDateTime>
#include <QSet>
#include <iostream>
#include <QSignalMapper>
#include <QMainWindow>
#include <QResizeEvent>
#include <QVBoxLayout>
#include <QLayout>
#include <QFont>
#include <QTimer>
#include <QProgressBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // colors

    maze_background_color = QBrush(QColor(28, 37, 65)); // grayish maze squares background  color
    wall_pen = QPen(Qt::black);  // square wall color - black
    wall_pen.setWidth(15); // width 15px
    start_and_path_background_color = QBrush(QColor(68, 102, 30)); // green
    last_square_background_color = QBrush(QColor(48, 12, 97)); // purple
    head = QBrush(Qt::red);


    qsrand(QDateTime::currentMSecsSinceEpoch() / 1000);

    ui->setupUi(this);

    int random_square_size = (qrand() & 80) + 20;
    maze_width = 1300 / random_square_size;
    maze_height = 700 / random_square_size;
    square_size = random_square_size;
    next_square_y = 0;
    next_square_x = 0;

    scene = new QGraphicsScene(this);
    scene->setBackgroundBrush(QColor(11, 19, 43));

    ui->graphicsView->setScene(scene);
    ui->graphicsView->setFrameStyle(0);

    QGraphicsRectItem *rect = new QGraphicsRectItem(0, 0, 782, 489);

    QPen no_border = QPen(Qt::NoPen);
    rect->setFlag(QGraphicsItem::ItemIsMovable);
    no_border.setWidth(0);
    rect->setPen(no_border);
    scene->addItem(rect);

    // walls will store square with 4 walls

    walls.resize(maze_width * maze_height);
    for (int y = 0; y < maze_height; y++){
        QVector < QGraphicsLineItem* > nothing;
        for (int x = 0; x < maze_width; x++){
            walls[y].push_back(nothing);
        }
    }

    // 0 walls are visited
    visited_walls.resize(maze_width * maze_height);
    for (int y = 0; y < maze_height; y++){
        for (int x = 0; x < maze_width; x++){
            visited_walls[y].push_back(0);
        }
    }

    current_square.resize(4);

    background_color_squares.resize(maze_width * maze_height);
    for (int y = 0; y < maze_height; y++){
        QGraphicsRectItem *nothing;
        for (int x = 0; x < maze_width; x++){
            background_color_squares[y].push_back(nothing);
        }
    }


    // makes maze grid
    for (int y = 0; y < maze_height; y++ ){
        for (int x = 0; x < maze_width; x ++){

            QGraphicsRectItem *background_color_square = new QGraphicsRectItem(x * square_size, y * square_size, square_size + 5, square_size + 5, rect);
            background_color_square->setPen(Qt::NoPen);
            background_color_squares[y][x] = background_color_square;

            // draw squares

            // top wall - 0
            QGraphicsLineItem *top_wall = new QGraphicsLineItem(x * square_size, y * square_size, (x * square_size) + square_size - 8, y * square_size, rect);
            top_wall->setPen(wall_pen);
            walls[y][x].push_back(top_wall);

            // left wall - 1
            QGraphicsLineItem *left_wall = new QGraphicsLineItem(x * square_size, y * square_size, x * square_size, (y * square_size) + square_size, rect);
            left_wall->setPen(wall_pen);
            walls[y][x].push_back(left_wall);

            // right wall - 2
            QGraphicsLineItem *right_wall = new QGraphicsLineItem((x * square_size) + square_size, y * square_size, (x * square_size) + square_size, (y * square_size) + square_size, rect);
            right_wall->setPen(wall_pen);
            walls[y][x].push_back(right_wall);

            // bottom wall - 3
            QGraphicsLineItem *bottom_wall = new QGraphicsLineItem(x * square_size, (y * square_size) + square_size, (x * square_size) + square_size - 3, (y * square_size) + square_size, rect);
            bottom_wall->setPen(wall_pen);
            walls[y][x].push_back(bottom_wall);

        }
    }

    // first square is visited
    current_square = walls[0][0];
    visited_walls[0][0] = 1;
    current_square_y = 0;
    current_square_x = 0;

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(MySlot()));

    timer1 = new QTimer(this);
    connect(timer1, SIGNAL(timeout()), this, SLOT(solve_slot()));

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    if (ui->radioButton->isChecked()){

        instant_maze_generation();

    }else {

        timer->start(1);

    }


}

void MainWindow::MySlot()
{
    // sets background color for the square
    background_color_squares[next_square_y][next_square_x]->setBrush(maze_background_color);
    background_color_squares[next_square_y][next_square_x]->setPen(Qt::NoPen);

    // start position - green square
    background_color_squares[0][0]->setBrush(start_and_path_background_color);

    // check for the next square
    next_square = check_neighbours(next_square_y, next_square_x);

    // if there isn't any neighbours around, go back
    while (next_square.size() == 0){

        memorize.pop_back();

        next_square_y = memorize[memorize.size() - 1].first;
        next_square_x = memorize[memorize.size() - 1].second;

        current_square_y = next_square_y;
        current_square_x = next_square_x;
        current_square = walls[current_square_y][current_square_x];

        next_square = check_neighbours(next_square_y, next_square_x);
    }
    // set new square background color to red
    background_color_squares[next_square_y][next_square_x]->setBrush(head);

    // mark new square as visited
    visited_walls[next_square_y][next_square_x] = 1;

    //  memorize every square coordinates
    QPair <int, int> memorized_cord = {next_square_y, next_square_x};
    memorize.push_back(memorized_cord);

    //  if the next square is under the current square, remove next square top wall and current square bottom wall.
    if (current_square_y - next_square_y == -1){

        current_square[3]->setPen(Qt::NoPen);
        next_square[0]->setPen(Qt::NoPen);
    }

    // if the next square is above the current square, remove next square bottom wall and current square top wall.
    if (current_square_y - next_square_y == 1){
        current_square[0]->setPen(Qt::NoPen);
        next_square[3]->setPen(Qt::NoPen);
    }

    // if the next square is at right side of the current square, remove next square left wall and current square right wall.
    if (current_square_x - next_square_x == -1){
        current_square[2]->setPen(Qt::NoPen);
        next_square[1]->setPen(Qt::NoPen);
    }

    // if the next square is at left side of the current square, remove next square right wall and current square left wall.
    if (current_square_x - next_square_x == 1){
        current_square[1]->setPen(Qt::NoPen);
        next_square[2]->setPen(Qt::NoPen);
    }

    // replaces curret_square to the next one.
    current_square = next_square;

    // replaces current_square coordinates to the next_square coordinates
    current_square_y = next_square_y;
    current_square_x = next_square_x;

    // if all squares are visited, end maze generation.
    if (!unvisited_squares()){
        background_color_squares[next_square_y][next_square_x]->setBrush(maze_background_color);
        background_color_squares[maze_height - 1][maze_width - 1]->setBrush(last_square_background_color);
        timer->stop();
    }


}


bool MainWindow::unvisited_squares()
{
    for (int y = 0; y < visited_walls.size(); y++){
        for (int x = 0; x < visited_walls[y].size(); x++){
            if (visited_walls[y][x] == 0){
                return true;
            }
        }
    }
    return false;
}

QVector < QGraphicsLineItem* > MainWindow::check_neighbours(int y, int x)
{
    QVector < QPair <int, int > > available_neighbours;

    // left neighbour
    if (x != 0){
        if (visited_walls[y][x - 1] != 1){
            QPair <int, int> neighbour = {y, x - 1};
            available_neighbours.push_back(neighbour);
        }
    }
    // right neighbour
    if (x != maze_width - 1){
        if (visited_walls[y][x + 1] != 1){
            QPair <int, int> neighbour = {y, x + 1};
            available_neighbours.push_back(neighbour);
        }
    }
    // above neighbour
    if (y != 0){
        if (visited_walls[y - 1][x] != 1){
            QPair <int, int> neighbour = {y - 1, x};
            available_neighbours.push_back(neighbour);
        }
    }
    // under
    if (y != maze_height - 1){
        if (visited_walls[y + 1][x] != 1){
            QPair <int, int> neighbour = {y + 1, x};
            available_neighbours.push_back(neighbour);
        }
    }

    // chooses randon neighbour from the available_neighbours and returns it
    if (available_neighbours.size() > 0){
        int random = (qrand() % available_neighbours.size());
        QPair <int, int> chosen_neighbour = available_neighbours[random];
        next_square_y = chosen_neighbour.first;
        next_square_x = chosen_neighbour.second;
        return walls[next_square_y][next_square_x];
    }
    return {};
}


void MainWindow::reset_background_color()
{
    for (int y = 0; y < maze_height; y++){
        for (int x = 0; x < maze_width; x++){
            background_color_squares[y][x]->setBrush(maze_background_color);
        }
    }
    background_color_squares[maze_height - 1][maze_width - 1]->setBrush(last_square_background_color);
}

void MainWindow::reset_visited_walls()
{
    for (int y = 0; y < maze_height; y++){
        for (int x = 0; x < maze_width; x++){
            visited_walls[y][x] = 0;
        }
    }
}

void MainWindow::reset_walls()
{
   // QPen wall_pen = QPen(Qt::black);
    //wall_pen.setWidth(15);
    for (int y = 0; y < maze_height; y++){
        for (int x = 0; x < maze_width; x++){
            for (int i = 0; i < 4; i++){
                walls[y][x][i]->setPen(wall_pen);
            }
        }
    }
}

// Solve button
void MainWindow::on_pushButton_2_clicked()
{
    reset_visited_walls();

    if (ui->radioButton->isChecked()){
        instant_solve();
    }else {

        next_square_y = 0;
        next_square_x = 0;
        current_square_y = 0;
        current_square_x = 0;
        timer1->start(10);
    }

}

void MainWindow::solve_slot()
{
    if (count == 0){
        background_color_squares[memorize_y][memorize_x]->setBrush(maze_background_color);
    }
    count++;
    background_color_squares[current_square_y][current_square_x]->setBrush(start_and_path_background_color);

    current_square_y = next_square_y;
    current_square_x = next_square_x;

    current_square = walls[current_square_y][current_square_x];
    background_color_squares[current_square_y][current_square_x]->setBrush(head);

    QPair <int, int> memorized_cord = {current_square_y, current_square_x};

    if (visited_walls[current_square_y][current_square_x] == 0){
        memorize_solve_function.push_back(memorized_cord);
    }
    visited_walls[current_square_y][current_square_x] = 1;
    next_square = legal_move(current_square_y, current_square_x);

    if (next_square.size() == 0){
        background_color_squares[current_square_y][current_square_x]->setBrush(head); // green - QColor(68, 102, 30)
        memorize_solve_function.pop_back();

        memorize_y = next_square_y;
        memorize_x = next_square_x;

        next_square_y = memorize_solve_function[memorize_solve_function.size() - 1].first;
        next_square_x = memorize_solve_function[memorize_solve_function.size() - 1].second;

        current_square_y = next_square_y;
        current_square_x = next_square_x;
        current_square = walls[current_square_y][current_square_x];

        next_square = legal_move(next_square_y, next_square_x);
       //
        count = 0;
    }

    if (next_square_y == maze_height - 1 && next_square_x == maze_width - 1){
        timer1->stop();
        background_color_squares[current_square_y][current_square_x]->setBrush(start_and_path_background_color);
        background_color_squares[memorize_y][memorize_x]->setBrush(maze_background_color);
        background_color_squares[maze_height - 1][maze_width - 1]->setBrush(head);
    }

}


QVector<QGraphicsLineItem *> MainWindow::legal_move(int y, int x)
{
    QPen top_wall = walls[y][x][0]->pen();
    QPen bottom_wall = walls[y][x][3]->pen();
    QPen left_wall = walls[y][x][1]->pen();
    QPen right_wall = walls[y][x][2]->pen();

    QVector < QPair<int, int > > walls_without_border;

    // stores above square coordinates, if meets conditions
    if (top_wall == Qt::NoPen && y != 0 && visited_walls[y - 1][x] == 0){
        walls_without_border.push_back({y - 1, x});
    }
    // stores lower square coordinates, if meets conditions
    if (bottom_wall == Qt::NoPen && y != maze_height - 1  && visited_walls[y + 1][x] == 0){
        walls_without_border.push_back({y + 1, x});
    }
    // stores left square coordinates, if meets conditions
    if (left_wall == Qt::NoPen && x != 0  && visited_walls[y][x - 1] == 0){
        walls_without_border.push_back({y, x - 1});
    }
    // stores right square coordinates, if meets conditions
    if (right_wall == Qt::NoPen && x != maze_width - 1  && visited_walls[y][x + 1] == 0){
        walls_without_border.push_back({y, x + 1});
    }
    if (walls_without_border.size() > 0){
        int random = (qrand() % walls_without_border.size());
        next_square_y = walls_without_border[random].first;
        next_square_x = walls_without_border[random].second;
        return walls[next_square_y][next_square_x];
    }else {
        return {};
    }


}

void MainWindow::instant_maze_generation()
{
    while (true) {
        // grayish color
        QBrush background_color = QBrush(maze_background_color);

        // sets background color for the square
        background_color_squares[next_square_y][next_square_x]->setBrush(background_color);
        background_color_squares[next_square_y][next_square_x]->setPen(Qt::NoPen);

        // start position - green square
        background_color_squares[0][0]->setBrush(start_and_path_background_color);

        // check for the next square
        next_square = check_neighbours(next_square_y, next_square_x);

        // if there isn't any neighbours around, go back
        while (next_square.size() == 0){

            memorize.pop_back();

            next_square_y = memorize[memorize.size() - 1].first;
            next_square_x = memorize[memorize.size() - 1].second;

            current_square_y = next_square_y;
            current_square_x = next_square_x;
            current_square = walls[current_square_y][current_square_x];

            next_square = check_neighbours(next_square_y, next_square_x);
        }
        // set new square background color to red
        background_color_squares[next_square_y][next_square_x]->setBrush(head);

        // mark new square as visited
        visited_walls[next_square_y][next_square_x] = 1;

        //  memorize every square coordinates
        QPair <int, int> memorized_cord = {next_square_y, next_square_x};
        memorize.push_back(memorized_cord);

        //  if the next square is under the current square, remove next square top wall and current square bottom wall.
        if (current_square_y - next_square_y == -1){

            current_square[3]->setPen(Qt::NoPen);
            next_square[0]->setPen(Qt::NoPen);
        }

        // if the next square is above the current square, remove next square bottom wall and current square top wall.
        if (current_square_y - next_square_y == 1){
            current_square[0]->setPen(Qt::NoPen);
            next_square[3]->setPen(Qt::NoPen);
        }

        // if the next square is at right side of the current square, remove next square left wall and current square right wall.
        if (current_square_x - next_square_x == -1){
            current_square[2]->setPen(Qt::NoPen);
            next_square[1]->setPen(Qt::NoPen);
        }

        // if the next square is at left side of the current square, remove next square right wall and current square left wall.
        if (current_square_x - next_square_x == 1){
            current_square[1]->setPen(Qt::NoPen);
            next_square[2]->setPen(Qt::NoPen);
        }

        // replaces curret_square to the next one.
        current_square = next_square;

        // replaces current_square coordinates to the next_square coordinates
        current_square_y = next_square_y;
        current_square_x = next_square_x;

        // if all squares are visited, end maze generation.
        if (!unvisited_squares()){
            background_color_squares[next_square_y][next_square_x]->setBrush(background_color);
            background_color_squares[maze_height - 1][maze_width - 1]->setBrush(last_square_background_color);
            break;
        }
    }



}

void MainWindow::instant_solve()
{
    current_square_x = 0;
    current_square_y = 0;
    next_square_x = 0;
    next_square_y = 0;
    while(true){

        current_square_x = next_square_x;
        current_square_y = next_square_y;

        current_square = walls[current_square_y][current_square_x];
        visited_walls[current_square_y][current_square_x] = 1;

        background_color_squares[current_square_y][current_square_x]->setBrush(start_and_path_background_color);

        QPair <int, int> memorized_cord = {current_square_y, current_square_x};
        memorize_solve_function.push_back(memorized_cord);

        next_square = legal_move(current_square_y, current_square_x);

        while (next_square.size() == 0){
            memorize_solve_function.pop_back();

            background_color_squares[current_square_y][current_square_x]->setBrush(maze_background_color);

            current_square_y = memorize_solve_function[memorize_solve_function.size() - 1].first;
            current_square_x = memorize_solve_function[memorize_solve_function.size() - 1].second;

            next_square = legal_move(current_square_y, current_square_x);
        }

        if (next_square_y == maze_height - 1 && next_square_x == maze_width - 1){
            background_color_squares[maze_height - 1][maze_width - 1]->setBrush(head);
            background_color_squares[0][0]->setBrush(Qt::blue);
            break;
        }
    }

}

void MainWindow::on_pushButton_3_clicked()
{
    reset_visited_walls();
    reset_background_color();
    memorize.resize(0);
    memorize_solve_function.resize(0);
    reset_walls();
}

void MainWindow::on_actiontest_triggered()
{
    maze_background_color = QBrush(QColor(253, 255, 252)); // grayish maze squares background  color
    wall_pen = QPen(QColor(35, 87, 137));  // square wall color - black
    wall_pen.setWidth(15); // width 15px
    start_and_path_background_color = QBrush(QColor(193, 41, 46)); // green
    last_square_background_color = QBrush(QColor(213, 109, 112)); // purple
    head = QBrush(QColor(199, 60, 65));
}

void MainWindow::on_actionDark_triggered()
{
        maze_background_color = QBrush(QColor(28, 37, 65));
        wall_pen = QPen(Qt::black);
        wall_pen.setWidth(15); // width 15px
        start_and_path_background_color = QBrush(QColor(68, 102, 30));
        last_square_background_color = QBrush(QColor(48, 12, 97));
}


