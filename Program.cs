using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ITiROD_Client
{
    static class Program
    {
        public static int IDClient { get; set; } // ID выдаваемое сервером
        public static Socket Client { get; set; }   // Сокет клиента
        public static IPAddress Ip { get; set; }    // IP адрес
        public static int Port { get; set; }   // Порт
        public static string NameClient { get; set; }   // Никнайм пользователя


        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new MainForm());
        }
    }
}
