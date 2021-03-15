using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Threading;
using System.Linq;

namespace Eclat
{

    public partial class Form1 : Form
    {

        private CSVReader reader;

        private EclatParallel eclatParallel;
        private EclatSerial eclatSerial;

        private BackgroundWorker worker;

        private string filePath = "C:\\Users\\vlad\\Desktop\\Eclat\\fm.csv";
        private int numberOfRowsInFile = 0;

        private CancellationTokenSource tokenSource;

        // Dataset info

        private List<string> fileLines;

        private CancellationToken token { get { return tokenSource.Token; } }

        public Form1()
        {
            InitializeComponent();

            reader = new CSVReader();

            eclatSerial = new EclatSerial();
            eclatParallel = new EclatParallel();
            tokenSource = new CancellationTokenSource();

            configureState(false);

            progressTextBox.Text = "Идет подсчет чтение данных из файла";
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            // empty
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            readLinesInFile();
        }

        private async void startButton_Click(object sender, EventArgs e)
        {
            int selectRows = 0;
            double support = 0;

            if (!int.TryParse(rowsNumberTextBox.Text, out selectRows) || !double.TryParse(supportTextBox.Text, out support))
            {
                MessageBox.Show("Должны быть только числа!");
                return;
            }

            if (selectRows <= 999 || selectRows > numberOfRowsInFile)
            {
                MessageBox.Show("Должно быть число строк от 1000 до " + numberOfRowsInFile);
                return;
            }

            if (support <= 0.009 || support > 1.0)
            {
                MessageBox.Show("Должно быть support от 0.010 до 1.0");
                return;
            }

            configureState(false);
            stopButton.Enabled = true;

            var progressEclatIndicator = new Progress<int>(eclatReportProgress);

            progressTextBox.Text = "Данные начали парситься!";

            var datasetInfo = await ParseDatasetAsync(selectRows);

            progressTextBox.Text = "Датасет считан! Начинает работу Eclat!";

            Stopwatch watch = new Stopwatch();
            watch.Restart();

            try
            {
                if (checkBox1.Checked)
                {
                    var result = await EclatParallelDatasetAsync(progressEclatIndicator, datasetInfo, support, token);
                    watch.Stop();
                    printResults(result, watch.Elapsed.TotalSeconds);
                }
                else
                {
                    var result = await EclatSerialDatasetAsync(progressEclatIndicator, datasetInfo, support, token);
                    watch.Stop();
                    printResults(result, watch.Elapsed.TotalSeconds);
                }
            }
            catch
            {
                printResults(null, 0);
            }

            configureState(true);
            stopButton.Enabled = false;
        }

        private void printResults(IDictionary<HashSet<String>, double> results, double time)
        {
            if (results == null)
            {
                progressTextBox.Text = "Обработка остановлена!";

                return;
            }

            previousResultTextBox.Text = currentResultTextBox.Text;

            currentResultTextBox.Text = checkBox1.Checked ? "Параллельный алгоритм\r\n" : "Последовательный алгоритм\r\n";

            currentResultTextBox.AppendText("\r\nПотрачено времени (сек): " + time + "\r\n");
            currentResultTextBox.AppendText("Всего элементов: " + results.Count + "\r\n");

            foreach (var value in results)
            {
                var param = string.Join(", ", value.Key);
                currentResultTextBox.AppendText("\r\n" + param + ": " + value.Value + "\r\n");
            }

            progressTextBox.Text = "Данные полностью обработаны!";
        }

        private void stopButton_Click(object sender, EventArgs e)
        {
            tokenSource.Cancel();

            tokenSource = new CancellationTokenSource();

            configureState(true);
            stopButton.Enabled = false;
        }

        void eclatReportProgress(int value)
        {
            progressTextBox.Text = "Обработано параметров: " + value;
        }

        private void configureState(bool isAvailableInterface)
        {
            startButton.Enabled = isAvailableInterface;
            stopButton.Enabled = isAvailableInterface;
            rowsNumberTextBox.Enabled = isAvailableInterface;
            checkBox1.Enabled = isAvailableInterface;
        }

        // MARK: - Elcat

        public async Task<(Dictionary<String, HashSet<int>>, int)> ParseDatasetAsync(int count)
        {
            return await Task.Run(() =>
            {
                return reader.parse(fileLines.Take(count).ToList());
            });
        }

        public async Task<Dictionary<HashSet<String>, double>> EclatSerialDatasetAsync(IProgress<int> progress,
            (Dictionary<String, HashSet<int>>, int) datasetInfo, double support, CancellationToken token)
        {
            return await Task.Run(() =>
            {
                return eclatSerial.StartProcess(datasetInfo.Item1, datasetInfo.Item2, support, progress, token);
            }, token);
        }

        public async Task<ConcurrentDictionary<HashSet<String>, double>> EclatParallelDatasetAsync(IProgress<int> progress,
            (Dictionary<String, HashSet<int>>, int) datasetInfo, double support, CancellationToken token)
        {
            return await Task.Run(() =>
            {
                return eclatParallel.StartProcess(datasetInfo.Item1, datasetInfo.Item2, support, progress, token);
            }, token);
        }

        // MARK: - Read lines

        private void readLinesInFile()
        {
            worker = new BackgroundWorker();
            worker.DoWork += workerReadLines;
            worker.RunWorkerCompleted += workerReadLinesCompleted;

            worker.RunWorkerAsync();
        }

        private void workerReadLines(object sender, DoWorkEventArgs e)
        {
            e.Result = reader.read(filePath);
        }

        void workerReadLinesCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            progressTextBox.Text = "Строки из файла считались!";

            fileLines = (List<string>)e.Result;
            numberOfRowsInFile = fileLines.Count;

            rowsAvailableLabel.Text = numberOfRowsInFile.ToString();

            rowsNumberTextBox.Enabled = true;
            checkBox1.Enabled = true;
        }

        // MARK: - UI

        private void rowsNumberTextBox_TextChanged(object sender, EventArgs e)
        {
            startButton.Enabled = rowsNumberTextBox.TextLength > 0;
        }
    }
}
