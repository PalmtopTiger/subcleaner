#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFileInfo>
#include <QDir>
#include <QTimer>
#include "cleaner.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName("SubCleaner");
    app.setApplicationVersion("2.0");
    app.setOrganizationName("Unlimited Web Works");

    QCommandLineParser parser;
    parser.setApplicationDescription("This program strips fonts, graphics and other useless information from SSA/ASS files.");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("input", "Input subtitle file.");
    parser.addPositionalArgument("output", "Output subtitle file.");

    const QCommandLineOption stripComments(QStringList() << "c" << "strip-comments",
                                           "Strip comments.");
    const QCommandLineOption stripStyleInfo(QStringList() << "i" << "strip-info",
                                            "Strip useless lines from info section.");
    parser.addOption(stripComments);
    parser.addOption(stripStyleInfo);

    parser.process(app);
    const QStringList args = parser.positionalArguments();

    if (args.isEmpty())
    {
        fprintf(stderr, "%s\n", qPrintable("Input file doesn't set."));
        ::exit(EXIT_FAILURE);
    }
    const QString inputFile = args.at(0);
    QString outputFile;

    if (args.length() >= 2)
    {
        outputFile = args.at(1);
    }
    else
    {
        const QFileInfo fileInfo(inputFile);
        QStringList name;
        name << fileInfo.baseName()
             << "clean";
        if (!fileInfo.completeSuffix().isEmpty())
        {
            name << fileInfo.completeSuffix();
        }
        outputFile = fileInfo.dir().filePath(name.join('.'));
    }

    Cleaner::Options flags;
    if ( parser.isSet(stripComments) )  flags |= Cleaner::StripComments;
    if ( parser.isSet(stripStyleInfo) ) flags |= Cleaner::StripStyleInfo;

    const Cleaner cleaner(&app, inputFile, outputFile, flags);
    QObject::connect(&cleaner, &Cleaner::finished, &app, &QCoreApplication::quit);
    QTimer::singleShot(0, &cleaner, &Cleaner::run);
    return app.exec();
}
