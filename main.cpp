#include <QtGui>
#include <string.h>
#include <functional>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QImage image;
    QSize size;
    QString file;
    QColor color(Qt::black);

    char buf[1024];
    auto checkCommand = [&](const char *needle, const std::function<void(const QString &)> &func) {
        const int len = strlen(needle);
        if (!strncmp(buf, needle, len) && buf[len] == ' ') {
            func(QString::fromLocal8Bit(buf + len + 1));
            if (size.isValid() && image.isNull()) {
                image = QImage(size, QImage::Format_ARGB32_Premultiplied);
                image.fill(qRgba(0, 0, 0, 0));
                // qDebug() << "creating" << size;
            }
            if (!image.isNull() && !file.isEmpty()) {
                image.save(file, "PNG");
            }
            return true;
        }
        return false;
    };
    while ((fgets(buf, sizeof(buf), stdin))) {
        int len = strlen(buf);
        if (len > 0)
            buf[--len] = '\0';
        (checkCommand("file", [&](const QString &fileName) { file = fileName; })
         || checkCommand("color", [&](const QString &col) { color = col; })
         || checkCommand("width", [&](const QString &width) { size.setWidth(width.toInt()); })
         || checkCommand("height", [&](const QString &height) { size.setHeight(height.toInt()); })
         || checkCommand("load", [&](const QString &filename) { image = QImage(filename); })
         || checkCommand("fill", [&](const QString &rect) {
                 QStringList keys = rect.split(" ", QString::SkipEmptyParts);
                 if (keys.size() == 4 && !image.isNull()) {
                     QPainter p(&image);
                     p.fillRect(QRect(keys.at(0).toInt(),
                                      keys.at(1).toInt(),
                                      keys.at(2).toInt(),
                                      keys.at(3).toInt()), color);
                 }
             })
         || checkCommand("line", [&](const QString &string) {
                 QStringList keys = string.split(" ", QString::SkipEmptyParts);
                 if (keys.size() == 4 && !image.isNull()) {
                     QPainter p(&image);
                     p.setPen(QPen(color));
                     p.drawLine(keys.at(0).toInt(),
                                keys.at(1).toInt(),
                                keys.at(2).toInt(),
                                keys.at(3).toInt());
                 }
             })

         || ([&]() { qDebug() << "Unknown command" << buf; return true; })());
    }

    return 0;
}
