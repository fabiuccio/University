using System;
using System.Collections.Generic;
using System.Text;
using System.Threading; //AutoresetSemaphore
using System.Net;
using System.Net.Sockets;
using System.Windows.Forms;

namespace VolumeLoggerUI
{
    /// <summary>
    /// Questa classe modella il comportamento di ricezione di dati ottenuti
    /// dalla rete (il pacchetto mandato in multicast dai servizi remoti).
    /// In quanto tale contiene un metodo StartRetrieval che è quello in cui
    /// si esplica la logica da assegnare all'opportuno thread.
    /// Questa classe contiene inoltre richiede un riferimento ad un VLForm
    /// in quanto si fa anche carico di richiedere l'update grafico ad ogni 
    /// ricezione del pacchetto con i nuovi dati.
    /// la classe DataReceiver contiene un flag (proprietà HasBeenLaunched) 
    /// non modificabile esternamente che traccia se StartRetrieval è stato 
    /// chiamato o meno. In questo modo si può discriminare, all'avvio del
    /// thread principale, se il thread è stato già lanciato o meno.
    /// Il flag "IsRunning" invece è utile per bloccare o meno la ricezione
    /// effettiva dei pacchetti dalla rete. 
    /// Settare improvvisamente il flag ta "true" a "false" comporta uno dei 
    /// seguenti comportamenti:
    /// - Se StartRetrieval è appeso alla socket in attesa di dati, vi resta
    ///   e alla ricezione del primo pacchetto utile lo scarta e si appende 
    ///   al semaforo in attesa che "IsRunning" torni true e il semaforo sia
    ///   segnalato.
    /// - Se StartRetrieval non è appeso alla socket, si appende direttamente
    ///   al semaforo in attesa che "IsRunning" torni true e il semaforo sia
    ///   segnalato.
    /// Il metodo Kill invece si occupa di chiudere la socket e conseguentemente
    /// di far terminare l'esecuzione di StartRetrieval. 
    /// Il metodo Kill va chiamato una volta sola e unicamente dopo che
    /// il metodo StartRetrieval sia stato chiamato.
    /// 
    /// Se non differentemente omunicato, l'applicazione comincia in stato
    /// isRunning=false, cioè non riceve pacchetti fino a richiesta contraria.
    /// Questo accade anche se si fa stop/restart. 
    /// E' possibile settare isRunning a true anche prima di lanciare l'appli-
    /// cazione in modo da offrire un servizio immediato. Se si setta la proprietà
    /// IsRunning prima di chiamare il metodo StartRetrieval allora verrà tenuta
    /// memoria della scelta effettuata
    /// </summary>
    class DataReceiver
    {
        private bool isRunning = false, hasBeenLaunched = false;
        private AutoResetEvent mySemaphore = new AutoResetEvent(false);
        private Socket multicastListener = null;
        private MainForm formRef;
        public bool HasBeenLaunched
        {
            get { return hasBeenLaunched; }
        }
        public bool IsRunning
        {
            get { return isRunning; }
            set
            {
                if (value != isRunning) //Idempotenza (inutile se buon 
                //controllo sui token)
                {
                    isRunning = value;

                    if (isRunning)
                    {
                        mySemaphore.Set();
                        //Segnala semaforo 
                        //se già nn segnalato (al massimo rilascia 1 token)
                    }
                    else
                    {
                        mySemaphore.Reset();
                        //Svuota eventuali token spuri dal semaforo
                        //(sempre per garantire al massimo 1 token rilasciato)
                    }
                }
            }
        }
        private string address = "239.255.255.250";
        public string Address
        {
            set
            {
                if (value != null)
                    address = value;
            }
            get
            {
                return address;
            }
        }
        private int port = 3001;
        public int Port
        {
            set
            {
                if (value > 0 && value < 65536)
                    port = value;
            }
            get
            {
                return port;
            }
        }

        public DataReceiver(MainForm f)
        {
            setDataReceiver(f, "239.255.255.250", 1900);
        }
        public DataReceiver(MainForm f, string address)
        {
            setDataReceiver(f, address, 1900);
        }
        public DataReceiver(MainForm f, string address, int port)
        {
            setDataReceiver(f, address, port);
        }
        private void setDataReceiver(MainForm f, string address, int port)
        {
            if (address != null)
                this.address = address;
            this.formRef = f;
            if (port > 0 && port < 65536) this.port = port;
        }
        /* Procedura di retrieval dalla rete */
        public void StartRetrieval()
        {
            hasBeenLaunched = true;
            IsRunning = true;

            byte[] b = new byte[1024];
            try
            {
                /* Init socket multicast */

                multicastListener = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
                IPEndPoint ipep = new IPEndPoint(IPAddress.Any, port);

                // Sets the socket in a non-exclusive state, thus allowing other 
                // process to bind to the same port. Finally, 
                multicastListener.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, 1);

                multicastListener.Bind(ipep);
                IPAddress ip = IPAddress.Parse(address);
                //join the specified multicast group on all network interfaces
                multicastListener.SetSocketOption(SocketOptionLevel.IP, SocketOptionName.AddMembership, new MulticastOption(ip, IPAddress.Any));
            }
            catch (Exception e)
            {
                MessageBox.Show("Connessione abortita. Eccezione inattesa:\n" + e.Message);
                if (multicastListener != null) multicastListener.Close();
                formRef.BeginInvoke(formRef.setStopDelegate, null);
                return;
            }
            try
            {
                while (true)
                {
                    if (IsRunning)
                    {
                        //Lettura dei pacchetti in multicast dalla porta UDP
                        EndPoint sender = new IPEndPoint(IPAddress.Any, 0);

                        int count = multicastListener.ReceiveFrom(b, ref sender);
                        if (IsRunning)
                        {
                            UInt32 sequence = System.BitConverter.ToUInt32(b, 0);

                            string datetime = "";
                            for (int i = 4; i < 16; i++)
                                datetime += (char)b[i];

                            string filename = "";
                            int o=0;
                            for (o= 16; o < 1024; o++)
                            {
                                if ((char)b[o] == '\0') break;
                                filename += (char)b[o];
                            }

                            //string processname = ""+System.BitConverter.ToInt32(b, o+1);
                            string processname = "";
                            for (int i = 16 + filename.Length + 1; i < 1024; i++)
                            {
                                if ((char)b[i] == '\0') break;
                                processname += (char)b[i];
                            }

                            string action = "";
                            for (int i = 16+filename.Length+processname.Length+3; i < 1024; i++)
                            {
                                if ((char)b[i] == '\0') break;
                                action += (char)b[i];
                            }

                            string subaction = "";
                            for (int i = 16 + filename.Length + processname.Length+action.Length+4; i < 1024; i++)
                            {
                                if ((char)b[i] == '\0') break;
                                subaction += (char)b[i];
                            }

                            string hostmaster=null;
                            string hostslave=null;
                            try
                            {
                                IPHostEntry remotehost = Dns.GetHostEntry(((IPEndPoint)sender).Address);
                                hostmaster = remotehost.HostName;
                                hostslave = "" + ((IPEndPoint)sender).Address + ":" + ((IPEndPoint)sender).Port;
                            }
                            catch (Exception e)
                            {
                                hostmaster = "" + ((IPEndPoint)sender).Address + ":" + ((IPEndPoint)sender).Port;
                                hostslave = null;
                            }

                            formRef.BeginInvoke(formRef.addRecordDelegate, new Object[] { new String[] { hostmaster, hostslave, ""+sequence, processname, datetime, filename, action, subaction } });

                            //writeout data received

                            //formRef.Invoke(formRef.myDelegate);
                            /*
                                sequence (int) 4 byte
                                HANDLE 4 byte 
                                LOCALTIME 12 BYTE
                                nomeprocesso \0
                                major \0
                                e  minor \0 [fac]
                            */
                        }
                        //else ignore them
                    }
                    else
                    {
                        //WaitOnSemaphore / WaitOne
                        mySemaphore.WaitOne();
                    }

                }// fine While: se chiamo Kill()

            }// fine try
            catch (Exception esv)
            {
                Console.WriteLine("Eccezione attesa su socket!" + esv.ToString());
                //MessageBox.Show("Retrieval dei dati interrotto:\n" + esv.ToString());

            }
            finally
            {
                //SE avessi posto isRunning=false nel metodo Kill allora non
                //si sarebbe sbloccato il while (semaforo segnalato+isRunning=false
                //significa di nuovo appendersi al semaforo). Quindi setto isRunning
                //a false QUI.
                isRunning = false;
            }
        }

        public void Kill()
        {
            hasBeenLaunched = false;
            isRunning = true;
            //SE avessi posto isRunning=false QUI allora non
            //si sarebbe sbloccato il while di StartRetrieval 
            //(semaforo segnalato+isRunning=false significa di nuovo appendersi 
            // al semaforo). Quindi setto isRunning a false una volta uscito dal while
            // di StartRetrieval
            try
            {
                if (multicastListener != null)
                    multicastListener.Close();
                mySemaphore.Set();
            }
            catch (Exception e)
            {
                Console.WriteLine(e.ToString());

            }
        }
    }/* End DataReceiver */
}
