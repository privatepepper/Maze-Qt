#ifndef GRAPH_H
#define GRAPH_H

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QtWidgets>



class Graph {

    private:
        int vertices;
        QList <int> *Adjacency_list;

    public:
        Graph(int V){
            this->vertices = V;
            Adjacency_list = new QList <int> [V];
        }
        void addEdge(int u, int v){
            Adjacency_list[u].push_back(v);
            Adjacency_list[v].push_back(u);
        }
        QList <int>  *get_list(){
            QList <int> *n_e_w = Adjacency_list;
            return n_e_w;
        }
//        void printGraph(){
//            for(int i=0;i<V;i++){
//                cout<<i<<"=>";
//                for(auto it:AdjList[i])
//                    cout<<it<<",";
//                cout<<"\n";
//            }
//        }
        QString printGraph(){
            QString s;
            for(int i = 0; i < vertices; i++){
                s.append(QString::number(i) + "=> ");
                for (auto it : Adjacency_list[i]){
                    s.append(QString::number(it) + ", ");
                }
                s.append("\n");

            }
            return s;
        }

};


#endif // GRAPH_H
