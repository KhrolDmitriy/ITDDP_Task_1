using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Windows.Forms;

namespace ITiROD_Client
{
    public partial class MainForm : Form
    {
        byte[] data;
        StringBuilder builder;
        int bytes = 0;
        string messToClient = "";
        string[] arrayMessTOClient = { "1", "2" };

        public MainForm()
        {
            InitializeComponent();
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            this.ControlBox = false;
        }

        private void ConnectionBtn_Click(object sender, EventArgs e)
        {
            try
            {
                if (Program.Client == null)
                {
                    string buffer = IPPortTB.Text;
                    string[] connect_info = buffer.Split(':');          // Разделяем ip и port
                    Program.Ip = IPAddress.Parse(connect_info[0]);      // Присваиваем ip
                    Program.Port = int.Parse(connect_info[1]);          // Присваиваем port

                    Program.Client = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                    Program.Client.Connect(Program.Ip, Program.Port);
                    CheckTimer.Enabled = true;
                    Program.NameClient = NameTB.Text;
                }
                else
                    MessageBox.Show("Вы уже подключились к серверу.");
            }
            catch
            {
                MessageBox.Show("Ошибка! Не удалось подключится к серверу.");
                Program.Client = null;
                CheckTimer.Enabled = false;
            }
        }

        private void SendBtn_Click(object sender, EventArgs e)
        {
            // message == "IDClient;NameCommand;NameClient;Arguments";
            string message = "";
            message = Program.IDClient.ToString() + ';';
            message += "sendMess;";
            message += Program.NameClient + ';';            
            message += SendMessTB.Text + ';';
            data = Encoding.ASCII.GetBytes(message);
            Program.Client.Send(data);
        }

        private void ExitBtn_Click(object sender, EventArgs e)
        {
            if(Program.Client != null)
                Program.Client.Shutdown(SocketShutdown.Both);
            Close();
        }

        private void CheckTimer_Tick(object sender, EventArgs e)
        {
            if (Program.Client.Available != 0)
            {
                data = new byte[256]; // буфер для ответа
                builder = new StringBuilder();
                messToClient = "";
                Array.Clear(arrayMessTOClient, 0, arrayMessTOClient.Length);
                bytes = 0;          // количество полученных байт

                do
                {
                    bytes = Program.Client.Receive(data, data.Length, 0);
                    builder.Append(Encoding.ASCII.GetString(data, 0, bytes));
                }
                while (Program.Client.Available > 0);


                messToClient = builder.ToString();
                arrayMessTOClient = messToClient.Split(';');

                // команды
                if (arrayMessTOClient[0] == "setYourID")
                {
                    Program.IDClient = Convert.ToInt32(arrayMessTOClient[1]);
                }
                else if (arrayMessTOClient[0] == "newMess")
                {
                    ChatRTB.AppendText(arrayMessTOClient[1]);
                }
                else MessageBox.Show("Ошибка! Получена не известная команда от сервера!");
            }
        }
    }
}
