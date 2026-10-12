#include <QCoreApplication>
#include <QRangeModel>
#include <QString>
#include <cstdio>
#include <tuple>
#include <vector>

using Row = std::tuple<QString, int>;

static void print(const QRangeModel &model, const char *title) {
    std::printf("%s\n", title);
    for (int r = 0; r < model.rowCount(); ++r) {
        const QString name = model.index(r, 0).data().toString();
        const int rating = model.index(r, 1).data().toInt();
        std::printf("  %-8s %d\n", qPrintable(name), rating);
    }
}

int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);

    std::vector<Row> rows = {
        {"Clang", 3}, {"GCC", 5}, {"MSVC", 4}, {"EDG", 2},
    };
    QRangeModel model(&rows);
    std::printf("Qt %s, columns: %d\n", qVersion(), model.columnCount());

    print(model, "as inserted");
    model.sort(1, Qt::DescendingOrder);
    print(model, "sort(1, Descending)");

    std::printf("underlying vector: %s %d first\n",
                qPrintable(std::get<0>(rows.front())), std::get<1>(rows.front()));
    return 0;
}
