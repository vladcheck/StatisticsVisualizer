#include "mainwindow.h"

bool areWeightsValid(const QVector<double>& weights, const QVector<double>& values) {
    return (weights.size() == values.size()) && !weights.isEmpty();
}

void MainWindow::createDataHeader(QWidget *statsPanel, QVBoxLayout *statsLayout)
{
    QLabel *mainHeader = new QLabel("Анализ данных", statsPanel);
    mainHeader->setStyleSheet("font-size: 32px; font-weight: 600; color: #ddd;");
    statsLayout->addWidget(mainHeader);
}

QWidget* MainWindow::createBasicDataSection(QWidget *parent, QLabel **elementCountLabel,
                                             QLabel **sumLabel, QLabel **averageLabel)
{
    QWidget *section = Draw::createStatSection(parent, "Основные метрики");
    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(section->layout());

    *elementCountLabel = Draw::createAndRegisterStatRow(section, layout, "Количество элементов", "0", "elementCountLabel");
    *sumLabel = Draw::createAndRegisterStatRow(section, layout, "Сумма", "0", "sumLabel");
    *averageLabel = Draw::createAndRegisterStatRow(section, layout, "Среднее арифметическое", "—", "averageLabel");

    return section;
}

QWidget* MainWindow::createMeansSection(QWidget *parent)
{
    QWidget *section = Draw::createStatSection(parent, "Средние");
    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(section->layout());

    m_geometricMeanLabel = Draw::createAndRegisterStatRow(section, layout, "Геом. среднее", "—", "geometricMeanLabel");
    m_harmonicMeanLabel = Draw::createAndRegisterStatRow(section, layout, "Гарм. среднее", "—", "harmonicMeanLabel");
    m_rmsLabel = Draw::createAndRegisterStatRow(section, layout, "Квадр. среднее", "—", "rmsLabel");
    m_trimmedMeanLabel = Draw::createAndRegisterStatRow(section, layout, "Усеч. среднее", "—", "trimmedMeanLabel");

    return section;
}

QWidget* MainWindow::createDistributionSection(QWidget *parent)
{
    QWidget *section = Draw::createStatSection(parent, "Распределение");
    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(section->layout());

    m_medianLabel = Draw::createAndRegisterStatRow(section, layout, "Медиана", "—", "medianLabel");
    m_modeLabel = Draw::createAndRegisterStatRow(section, layout, "Мода", "—", "modeLabel");
    m_stdDevLabel = Draw::createAndRegisterStatRow(section, layout, "Стандартное отклонение", "—", "stdDevLabel");
    m_skewnessLabel = Draw::createAndRegisterStatRow(section, layout, "Асимметрия", "—", "skewnessLabel");
    m_kurtosisLabel = Draw::createAndRegisterStatRow(section, layout, "Эксцесс", "—", "kurtosisLabel");
    m_madLabel = Draw::createAndRegisterStatRow(section, layout, "Медианное абсолютное отклонение", "—", "madLabel");
    m_robustStdLabel = Draw::createAndRegisterStatRow(section, layout, "Робастный стандартный разброс", "—", "robustStdLabel");
    m_shapiroWilkLabel = Draw::createAndRegisterStatRow(section, layout, "Шапиро-Уилк", "—", "shapiroWilkLabel");
    m_densityLabel = Draw::createAndRegisterStatRow(section, layout, "Плотность", "—", "densityLabel");
    m_chiSquareLabel = Draw::createAndRegisterStatRow(section, layout, "χ²-критерий", "—", "chiSquareLabel");
    m_kolmogorovLabel = Draw::createAndRegisterStatRow(section, layout, "Колмогоров-Смирнов", "—", "kolmogorovLabel");

    return section;
}

QWidget* MainWindow::createExtremesSection(QWidget *parent)
{
    QWidget *section = Draw::createStatSection(parent, "Экстремумы");
    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(section->layout());

    m_minLabel = Draw::createAndRegisterStatRow(section, layout, "Минимум", "—", "minLabel");
    m_maxLabel = Draw::createAndRegisterStatRow(section, layout, "Максимум", "—", "maxLabel");
    m_rangeLabel = Draw::createAndRegisterStatRow(section, layout, "Размах", "—", "rangeLabel");

    return section;
}

QWidget* MainWindow::setupDataPanel(QWidget *parent, QLabel **elementCountLabel,
                                     QLabel **sumLabel, QLabel **averageLabel)
{
    QWidget *statsPanel = new QWidget(parent);
    statsPanel->setLayout(new QVBoxLayout(statsPanel));
    Draw::setSizePolicyExpanding(statsPanel);

    QVBoxLayout *statsLayout = qobject_cast<QVBoxLayout *>(statsPanel->layout());
    statsLayout->setContentsMargins(12, 8, 12, 8);
    statsLayout->setSpacing(8);

    createDataHeader(statsPanel, statsLayout);
    statsLayout->addWidget(createBasicDataSection(statsPanel, elementCountLabel, sumLabel, averageLabel));
    statsLayout->addWidget(createMeansSection(statsPanel));
    statsLayout->addWidget(createDistributionSection(statsPanel));
    statsLayout->addWidget(createExtremesSection(statsPanel));

    statsLayout->addStretch();
    return statsPanel;
}

QScrollArea *setupDataSectionScrollArea(QWidget *parent, QWidget *toScroll) {
    QScrollArea *scrollArea = new QScrollArea(parent);
    scrollArea->setWidget(toScroll);
    scrollArea->setWidgetResizable(true);
    scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scrollArea->setFrameShape(QFrame::NoFrame); // Убираем рамку
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Горизонтальную отключаем
    scrollArea->setVerticalScrollBar(new QScrollBar(Qt::Vertical, scrollArea));
    scrollArea->verticalScrollBar()->setContextMenuPolicy(Qt::NoContextMenu);
    return scrollArea;
}

QWidget* MainWindow::setupTablePanel(QWidget *parent) {
    QWidget *tableSection = new QWidget(parent);

    this->m_table = Draw::setupTable(tableSection); // Создаем таблицу и возвращаем через outTable
    auto *tableToolbar = setupTableToolbar(tableSection, m_table);

    QVBoxLayout *tableSectionLayout = new QVBoxLayout(tableSection);
    tableSectionLayout->addWidget(tableToolbar);
    tableSectionLayout->addWidget(m_table);

    return tableSection;
}

QWidget* MainWindow::setupDataSection(QWidget* parent) {
    QWidget* dataSection = new QWidget(parent);
    QHBoxLayout* dataSectionLayout = new QHBoxLayout(dataSection);
    dataSectionLayout->setContentsMargins(0, 0, 0, 0);

    // Панель статистики с автоматическим растяжением
    QWidget* statsPanel = setupDataPanel(dataSection, &m_elementCountLabel,
                                         &m_sumLabel, &m_averageLabel);
    statsPanel->setMinimumWidth(320); // Минимальная ширина для статистики

    // Область прокрутки с новыми настройками
    QScrollArea* scrollArea = setupDataSectionScrollArea(dataSection, statsPanel);

    // Панель таблицы с возможностью растяжения
    QWidget* tablePanel = setupTablePanel(dataSection);
    tablePanel->setMinimumWidth(400);

    dataSectionLayout->addWidget(scrollArea, 1);  // Растягиваем сначала статистику
    dataSectionLayout->addWidget(tablePanel, 2);  // Затем таблицу в соотношении 1:2

    // Обработчик изменения размера для динамической прокрутки
    QObject::connect(scrollArea->verticalScrollBar(), &QScrollBar::rangeChanged,
                     [scrollArea](int min, int max) {
                         if (max > 0) {
                             scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
                         } else {
                             scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                         }
                     });

    return dataSection;
}

QWidget* MainWindow::setupTableToolbar(QWidget* parent, QTableWidget* table) {
    QWidget *toolbar = new QWidget(parent);
    toolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QHBoxLayout *toolbarLayout = new QHBoxLayout(toolbar);
    toolbarLayout->setSpacing(5);

    // Спинбоксы
    auto *columnsContainer = Draw::createSpinBoxWithLabel(toolbar, "Столбцы", 512, initialColCount);
    QSpinBox *colSpinBox = qobject_cast<QSpinBox *>(columnsContainer->layout()->itemAt(1)->widget());

    this->m_table = table;
    this->m_addColBtn = Draw::createToolButton("Добавить столбец", "add-column");
    this->m_delColBtn = Draw::createToolButton("Удалить столбец", "delete-column");
    this->m_clearBtn = Draw::createToolButton("Очистить", "clear");
    this->m_autoSizeBtn = Draw::createToolButton("Авторазмер", "auto-size");
    this->m_colSpin = colSpinBox;
    this->m_importBtn = Draw::createToolButton("Импортировать данные", "import-file");
    this->m_exportBtn = Draw::createToolButton("Экспортировать данные", "export-file");

    setupTableActions();
    toolbarLayout->addLayout(columnsContainer);

    QList<QWidget*> toolbarWidgets = {
        m_addColBtn,
        m_delColBtn,
        m_autoSizeBtn,
        m_clearBtn,
        m_importBtn,
        m_exportBtn
    };

    // Добавляем элементы в layout
    for (QWidget *widget : toolbarWidgets) {
        toolbarLayout->addWidget(widget);
    }

    toolbarLayout->addStretch();

    return toolbar;
}

void MainWindow::setupTableActions()
{
    // Добавление столбца
    Draw::connect(m_addColBtn, [=]()
                  {
                      m_table->setColumnCount(m_table->columnCount() + 1);
                      m_colSpin->setValue(m_table->columnCount()); });

    // Удаление столбца
    Draw::connect(m_delColBtn, [=]()
                  {
                      if(m_table->columnCount() > 1) {
                          m_table->setColumnCount(m_table->columnCount() - 1);
                          m_colSpin->setValue(m_table->columnCount());
                      } });

    // Очистка таблицы
    Draw::connect(m_clearBtn, [=]()
                  {
                      auto reply = QMessageBox::question(
                          m_table,
                          "Очистка таблицы",
                          "Удалить все данные?",
                          QMessageBox::Yes | QMessageBox::No
                          );

                      if (reply == QMessageBox::Yes) {
                          m_table->clearContents();
                          m_colSpin->setValue(m_colSpin->minimum());
                      } });

    // Авторазмер
    Draw::connect(m_autoSizeBtn, [=]()
                  {
                      m_table->resizeColumnsToContents();
                      m_table->resizeRowsToContents(); });

    // Обработка изменения спинбокса столбцов
    Draw::connect(m_colSpin, [=](int value)
                  {
                      if (value >= m_colSpin->minimum()) {
                          m_table->setColumnCount(value);
                      } });

    // Импорт файлов
    QObject::connect(m_importBtn, &QPushButton::clicked, [=]() {
        Import::importFile(m_table);
    });
}

bool MainWindow::areAllLabelsDefined() {
    return (m_table &&
            m_elementCountLabel && m_sumLabel && m_averageLabel && m_geometricMeanLabel &&
            m_medianLabel && m_modeLabel && m_stdDevLabel && m_minLabel && m_maxLabel &&
            m_rangeLabel && m_skewnessLabel && m_kurtosisLabel && m_harmonicMeanLabel &&
            m_rmsLabel && m_trimmedMeanLabel && m_robustStdLabel && m_madLabel &&
            m_shapiroWilkLabel && m_kolmogorovLabel && m_chiSquareLabel && m_densityLabel);
}

TableRow MainWindow::parse() const {
    TableRow result;
    result.isValid = false;

    for (int col = 0; col < m_table->columnCount(); ++col) {
        QTableWidgetItem* item = m_table->item(0, col);
        if (!item || item->text().isEmpty()) continue;

        bool ok;
        double value = item->text().toDouble(&ok);
        if (ok) result.values.append(value);
    }

    result.isValid = !result.values.isEmpty();
    return result;
}

void MainWindow::updateUI(const TableRow& rowData) {
    const bool hasData = rowData.isValid;
    double mean = hasData ? Calculate::getMean(rowData.values) : 0.0;
    double stdDev = hasData ? Calculate::getStandardDeviation(rowData.values, mean) : 0.0;
    double min = hasData ? *std::min_element(rowData.values.begin(), rowData.values.end()) : 0.0;
    double max = hasData ? *std::max_element(rowData.values.begin(), rowData.values.end()) : 0.0;
    double range = max - min;

    // Основные метрики
    m_elementCountLabel->setText(hasData ? QString::number(rowData.values.size()) : na);
    m_sumLabel->setText(hasData ? QString::number(Calculate::getSum(rowData.values), 'f', statsPrecision) : na);
    m_averageLabel->setText(hasData ? QString::number(hasData ? mean : 0.0, 'f', statsPrecision) : na);

    // Средние значения
    m_geometricMeanLabel->setText(hasData ? QString::number(Calculate::geometricMean(rowData.values), 'f', statsPrecision) : na);
    m_harmonicMeanLabel->setText(hasData ? QString::number(Calculate::harmonicMean(rowData.values), 'f', statsPrecision) : na);
    m_rmsLabel->setText(hasData ? QString::number(Calculate::rootMeanSquare(rowData.values), 'f', statsPrecision) : na);
    m_trimmedMeanLabel->setText(hasData ? QString::number(Calculate::trimmedMean(rowData.values, trimmedMeanPercentage), 'f', statsPrecision) : na);

    // Распределение
    m_medianLabel->setText(hasData ? QString::number(Calculate::getMedian(rowData.values), 'f', statsPrecision) : na);
    m_modeLabel->setText(hasData ? QString::number(Calculate::getMode(rowData.values), 'f', statsPrecision) : na);
    m_stdDevLabel->setText(hasData ? QString::number(stdDev, 'f', statsPrecision) : na);
    m_skewnessLabel->setText(hasData ? QString::number(Calculate::skewness(rowData.values, mean, stdDev), 'f', statsPrecision) : na);
    m_kurtosisLabel->setText(hasData ? QString::number(Calculate::kurtosis(rowData.values, mean, stdDev), 'f', statsPrecision) : na);
    m_madLabel->setText(hasData ? QString::number(Calculate::medianAbsoluteDeviation(rowData.values), 'f', statsPrecision) : na);
    m_robustStdLabel->setText(hasData ? QString::number(Calculate::robustStandardDeviation(rowData.values), 'f', statsPrecision) : na);

    // Статистические тесты
    m_shapiroWilkLabel->setText(hasData ? QString::number(Calculate::shapiroWilkTest(rowData.values), 'f', statsPrecision) : na);
    m_densityLabel->setText(hasData ? QString::number(Calculate::calculateDensity(rowData.values, mean), 'f', statsPrecision) : na);
    m_chiSquareLabel->setText(hasData ? QString::number(Calculate::chiSquareTest(rowData.values), 'f', statsPrecision) : na);
    m_kolmogorovLabel->setText(hasData ? QString::number(Calculate::kolmogorovSmirnovTest(rowData.values), 'f', statsPrecision) : na);

    // Экстремумы
    m_minLabel->setText(hasData ? QString::number(min, 'f', statsPrecision) : na);
    m_maxLabel->setText(hasData ? QString::number(max, 'f', statsPrecision) : na);
    m_rangeLabel->setText(hasData ? QString::number(range, 'f', statsPrecision) : na);
}

void MainWindow::updateStatistics() {
    if (!areAllLabelsDefined()) return;

    const TableRow rowData = parse();
    updateUI(rowData);
}


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    QWidget *mainWidget = new QWidget(this);
    setCentralWidget(mainWidget);

    auto *header = Draw::setupHeader(mainWidget, 20);
    auto *dataSection = setupDataSection(mainWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->addWidget(header);
    mainLayout->addWidget(dataSection, 1);

    // Проверка инициализации перед подключением сигналов
    if (m_table) {
        // Обработчик изменения текущей ячейки
        connect(m_table, &QTableWidget::currentCellChanged, [this](int row, int, int, int) {
            updateStatistics();
        });

        // Обработчик изменения данных
        connect(m_table, &QTableWidget::itemChanged, this, &MainWindow::updateStatistics);
        updateStatistics();
    } else {
        qFatal("Table initialization failed!");
    }

    this->setWindowState(Qt::WindowMaximized);
    this->setWindowIcon(QIcon(":/logo.png"));
}

MainWindow::~MainWindow() {}
