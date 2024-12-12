using System.IO.MemoryMappedFiles;
using System.Text;

namespace CSClient
{
    public partial class Form1 : Form
    {
        const string SharedMemoryName = "MSG_EXCH";
        const int BufferSize = 100 * 1024 * 2;
        const int TotalSize = (BufferSize) + 8;

        MemoryMappedFile mmf;
        MemoryMappedViewAccessor accessor;

        byte[] buffer = new byte[BufferSize];

        private Thread readerThread;

        public Form1()
        {
            InitializeComponent();

            mmf = MemoryMappedFile.CreateOrOpen(SharedMemoryName, TotalSize);
            accessor = mmf.CreateViewAccessor();

            readerThread = new Thread(MessageReaderThread)
            {
                IsBackground = true
            };
            readerThread.Start();
        }

        private void MessageReaderThread()
        {
            while (true)
            {
                int isNewMessage = accessor.ReadInt32(0);

                if (isNewMessage == 1)
                {
                    // Считать сообщение
                    byte[] buffer = new byte[BufferSize];
                    accessor.ReadArray(8, buffer, 0, BufferSize);

                    string message = Encoding.Unicode.GetString(buffer).TrimEnd('\0');

                    // Сбросить флаг
                    accessor.Write(0, 0);

                    // Обновить интерфейс (обновления должны быть в основном потоке)
                    BeginInvoke(new Action(() =>
                    {
                        msg_label.Text = message;
                    }));
                }

                Thread.Sleep(100); // Избегаем активного ожидания
            }
        }

        private void textBox1_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                string input = textBox1.Text;
                msg_label.Text = input;
                buffer = Encoding.Unicode.GetBytes(input);

                byte[] clearBuffer = new byte[BufferSize];
                accessor.WriteArray(8, clearBuffer, 0, Buffer.ByteLength(clearBuffer));

                accessor.WriteArray(8, buffer, 0, Buffer.ByteLength(buffer));
                accessor.Write(4, 1);
            }
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            readerThread?.Interrupt();
            accessor.Dispose();
            mmf.Dispose();
        }
    }
}
