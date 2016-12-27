using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Threading;

namespace WpfApplication1
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private int time = 7;
        private DispatcherTimer Timer;

        private int tasks = 0;


        public MainWindow()
        {
            Timer = new DispatcherTimer();
            Timer.Interval = new TimeSpan(0, 0, 1);
            Timer.Tick += Timer_Tick;


            


            InitializeComponent();
            TBCountDown.Visibility = Visibility.Collapsed;
            comboBoxtime.Items.Add("seconds");
            comboBoxtime.Items.Add("minutes");            
            comboBoxtime.Items.Add("hours");
            comboBoxtime.SelectedItem = "minutes";
            checkBox0.IsChecked = true;
            radio1.IsChecked = true;
            textBox2.IsEnabled = false;
            textBox3.IsEnabled = false;
            percento1.IsEnabled = false;
            percento2.IsEnabled = false;

            button7.Visibility = Visibility.Collapsed;
            rbutton7.Visibility = Visibility.Collapsed;

            bell_png.Visibility = Visibility.Collapsed;
            button5.Visibility = Visibility.Visible;
            expander.Visibility = Visibility.Collapsed;
            expander1.Visibility = Visibility.Collapsed;
            expander2.Visibility = Visibility.Collapsed;
            expander3.Visibility = Visibility.Collapsed;

            sleepin.Visibility = Visibility.Collapsed;
            sleepin_label.Visibility = Visibility.Collapsed;
            hibin.Visibility = Visibility.Collapsed;
            hibins.Visibility = Visibility.Collapsed;
            lcknow.Visibility = Visibility.Collapsed;
            label.Visibility = Visibility.Collapsed;
            label2.Visibility = Visibility.Collapsed;
            logoutin.Visibility = Visibility.Collapsed;
            lckin.Visibility = Visibility.Collapsed;


            rcomboBoxtime.Items.Add("seconds");
            rcomboBoxtime.Items.Add("minutes");
            rcomboBoxtime.Items.Add("hours");
            rcomboBoxtime.SelectedItem = "minutes";
            rcheckBox0.IsChecked = true;
            rradio1.IsChecked = true;

            scomboBoxtime.Items.Add("seconds");
            scomboBoxtime.Items.Add("minutes");
            scomboBoxtime.Items.Add("hours");
            scomboBoxtime.SelectedItem = "minutes";
            scheckBox0.IsChecked = true;
            sradio1.IsChecked = true;

            lcomboBoxtime.Items.Add("seconds");
            lcomboBoxtime.Items.Add("minutes");
            lcomboBoxtime.Items.Add("hours");
            lcomboBoxtime.SelectedItem = "minutes";

            lcomboBoxtime1.Items.Add("seconds");
            lcomboBoxtime1.Items.Add("minutes");
            lcomboBoxtime1.Items.Add("hours");
            lcomboBoxtime1.SelectedItem = "minutes";
            lcheckBox0.IsChecked = true;
            lradio1.IsChecked = true;
        }

        void Timer_Tick(object sender, EventArgs e)
        {
            if (time > 0)
            {
                time--;
                TBCountDown.Content = string.Format("{0}",  time % 60);
                sleepin.Content = string.Format("{0}", time % 60);
                hibins.Content = string.Format("{0}", time % 60);
                lckin.Content = string.Format("{0}", time % 60);
                logoutin.Content = string.Format("{0}", time % 60);
            }
            else
            {
                Application.Current.Shutdown();
                Timer.Stop();
            }
        }


        // CPU PART
        private void checkBox_Checked(object sender, RoutedEventArgs e)
        {
            rekt2.Stroke = Brushes.Black;
            CPULABEL.Foreground = Brushes.Black;
            radio3.IsEnabled = true;
            radio4.IsEnabled = true;
        }


        private void checkBox_Unchecked(object sender, RoutedEventArgs e)
        {
            rekt2.Stroke = Brushes.Gray;
            CPULABEL.Foreground = Brushes.Gray;
            radio3.IsEnabled = false;
            radio4.IsEnabled = false;
            textBox2.IsEnabled = false;
            textBox3.IsEnabled = false;
            percento1.IsEnabled = false;
            percento2.IsEnabled = false;
        }

        private void checkBox0_Checked(object sender, RoutedEventArgs e)
        {
            rekt1.Stroke = Brushes.Black;
            radio1.IsEnabled = true;
            radio2.IsEnabled = true;
            TIMELABEL.Foreground = Brushes.Black;
        }

        private void checkBox0_Unchecked(object sender, RoutedEventArgs e)
        {
            rekt1.Stroke = Brushes.Gray;
            radio1.IsEnabled = false;
            radio2.IsEnabled = false;
            textBox.IsEnabled = false;
            textBox1.IsEnabled = false;
            comboBoxtime.IsEnabled = false;
            TIMELABEL.Foreground = Brushes.Gray;
        }

        private void radio1_Checked(object sender, RoutedEventArgs e)
        {
            textBox.IsEnabled = true;
            

        }

        private void radio1_Unchecked(object sender, RoutedEventArgs e)
        {
            textBox.IsEnabled = false;
            
        }

        private void radio2_Unchecked(object sender, RoutedEventArgs e)
        {
            textBox1.IsEnabled = false;
            comboBoxtime.IsEnabled = false;
        }

        private void radio2_Checked(object sender, RoutedEventArgs e)
        {
            textBox1.IsEnabled = true;
            comboBoxtime.IsEnabled = true;
        }

        private void radio3_Checked(object sender, RoutedEventArgs e)
        {
            textBox3.IsEnabled = true;
            percento1.IsEnabled = true;
        }

        private void radio3_Unchecked(object sender, RoutedEventArgs e)
        {
            textBox3.IsEnabled = false;
            percento1.IsEnabled = false;
        }

        private void radio4_Checked(object sender, RoutedEventArgs e)
        {
            textBox2.IsEnabled = true;
            percento2.IsEnabled = true;
        }

        private void radio4_Unchecked(object sender, RoutedEventArgs e)
        {
            textBox2.IsEnabled = false;
            percento2.IsEnabled = false;
        }

      //---------------------------





        private void rradio1_Checked(object sender, RoutedEventArgs e)
        {
            rtextBox.IsEnabled = true;


        }

        private void rradio1_Unchecked(object sender, RoutedEventArgs e)
        {
            rtextBox.IsEnabled = false;

        }

        private void rradio2_Unchecked(object sender, RoutedEventArgs e)
        {
            rtextBox1.IsEnabled = false;
            rcomboBoxtime.IsEnabled = false;
        }

        private void rradio2_Checked(object sender, RoutedEventArgs e)
        {
            rtextBox1.IsEnabled = true;
            rcomboBoxtime.IsEnabled = true;
        }

        private void rcheckBox0_Unchecked_1(object sender, RoutedEventArgs e)
        {
            rrekt1.Stroke = Brushes.Gray;
            rradio1.IsEnabled = false;
            rradio2.IsEnabled = false;
            rtextBox.IsEnabled = false;
            rtextBox1.IsEnabled = false;
            rcomboBoxtime.IsEnabled = false;
            rTIMELABEL.Foreground = Brushes.Gray;
        }

        private void rcheckBox0_Checked_1(object sender, RoutedEventArgs e)
        {
            rrekt1.Stroke = Brushes.Black;
            rradio1.IsEnabled = true;
            rradio2.IsEnabled = true;
            rTIMELABEL.Foreground = Brushes.Black;
        }





        //sleep tab


        private void scheckBox0_Checked(object sender, RoutedEventArgs e)
        {
            srekt1.Stroke = Brushes.Black;
            sradio1.IsEnabled = true;
            sradio2.IsEnabled = true;
            sTIMELABEL.Foreground = Brushes.Black;
        }

        private void scheckBox0_Unchecked(object sender, RoutedEventArgs e)
        {
            srekt1.Stroke = Brushes.Gray;
            sradio1.IsEnabled = false;
            sradio2.IsEnabled = false;
            stextBox.IsEnabled = false;
            stextBox1.IsEnabled = false;
            scomboBoxtime.IsEnabled = false;
            sTIMELABEL.Foreground = Brushes.Gray;
        }

        private void sradio1_Unchecked(object sender, RoutedEventArgs e)
        {
            stextBox.IsEnabled = false;
            
        }

        private void sradio1_Checked(object sender, RoutedEventArgs e)
        {
            stextBox.IsEnabled = true;
        }

        private void sradio2_Checked(object sender, RoutedEventArgs e)
        {
            stextBox1.IsEnabled = true;
            scomboBoxtime.IsEnabled = true;
        }

        private void sradio2_Unchecked(object sender, RoutedEventArgs e)
        {
            stextBox1.IsEnabled = false;
            scomboBoxtime.IsEnabled = false;
        }


        //lock tab

        private void lcheckBox0_Unchecked(object sender, RoutedEventArgs e)
        {
            lrekt1.Stroke = Brushes.Gray;
            lradio1.IsEnabled = false;
            lradio2.IsEnabled = false;
            lradio3.IsEnabled = false;
            ltextBox.IsEnabled = false;
            ltextBox1.IsEnabled = false;
            ltextBox2.IsEnabled = false;
            lcomboBoxtime.IsEnabled = false;
            lcomboBoxtime1.IsEnabled = false;
            lTIMELABEL.Foreground = Brushes.Gray;
        }

        private void lcheckBox0_Checked(object sender, RoutedEventArgs e)
        {
            lrekt1.Stroke = Brushes.Black;
            lradio1.IsEnabled = true;
            lradio2.IsEnabled = true;
            lradio3.IsEnabled = true;
            lTIMELABEL.Foreground = Brushes.Black;
        }

        private void lradio1_Checked(object sender, RoutedEventArgs e)
        {
            ltextBox.IsEnabled = true;
        }

        private void lradio1_Unchecked(object sender, RoutedEventArgs e)
        {
            ltextBox.IsEnabled = false;
        }

        private void lradio2_Unchecked(object sender, RoutedEventArgs e)
        {
            ltextBox1.IsEnabled = false;
            lcomboBoxtime.IsEnabled = false;
        }

        private void lradio2_Checked(object sender, RoutedEventArgs e)
        {
            ltextBox1.IsEnabled = true;
            lcomboBoxtime.IsEnabled = true;
        }

        private void lradio3_Checked(object sender, RoutedEventArgs e)
        {
            ltextBox2.IsEnabled = true;
            lcomboBoxtime1.IsEnabled = true;
        }

        private void lradio3_Unchecked(object sender, RoutedEventArgs e)
        {
            ltextBox2.IsEnabled = false;
            lcomboBoxtime1.IsEnabled = false;
        }

        //QUIT BUTTONS
        private void button_Click(object sender, RoutedEventArgs e)
        {
            Application.Current.Shutdown();
        }

        private void rbutton_Click(object sender, RoutedEventArgs e)
        {
            Application.Current.Shutdown();
        }

        private void sbutton_Click(object sender, RoutedEventArgs e)
        {
            Application.Current.Shutdown();
        }

        private void lbutton_Click(object sender, RoutedEventArgs e)
        {
            Application.Current.Shutdown();
        }

        private void tbutton_Click(object sender, RoutedEventArgs e)
        {
            Application.Current.Shutdown();
        }


        //SHTDWN OK
        private void button1_Click(object sender, RoutedEventArgs e)
        {
            string text = textBox.Text;
            string text1 = textBox1.Text;
            string units = comboBoxtime.Text;
            string per = textBox3.Text;
            string per2 = textBox2.Text;

            string msg = "";

            //timed shutdown
            if (checkBox0.IsChecked.Value)
            {
                //AT
                if(radio1.IsChecked.Value)
                {
                    if(text != "HH:MM" && text != "")
                    {
                        msg = "Computer will shutdown at: " + text;
                        //MessageBox.Show("Computer will shutdown at: " + text);
                    }
                }
                //IN
                else
                {
                    if(text1 != "")
                    {
                        msg = "Computer will shutdown in: " + text1 + " " + units;
                       // MessageBox.Show("Computer will shutdown in: " + text1 + " " + units + ".");
                    }
                    
                }
                
            }
            // CPU
            if (checkBox.IsChecked.Value)
            {
                //time + cpu
                if(checkBox0.IsChecked.Value)
                {
                    //exceeds
                    if(radio3.IsChecked.Value)
                    {
                        msg = msg + " or if CPU usage exceeds " + per + "%" ;
                    }
                    else
                    {
                        msg = msg + " or if CPU usage falls under " + per2 + "%";
                    }
                }
                //cpu only
                else
                {
                    if (radio3.IsChecked.Value)
                    {
                        msg = "Shutdown if CPU usage exceeds " + per + "%";
                    }
                    else
                    {
                        msg = "Shutdown if CPU usage falls under " + per2 + "%";
                    }
                }
            }
            if(msg != "")
            {
                msg = msg + ".";
                MessageBox.Show(msg);
                //task added
                tasks++;
                if(tasks == 1)
                {
                    header.Content = "You have " + tasks + " task scheduled:";
                }
                else if(tasks > 1)
                {
                    header.Content = "You have " + tasks + " tasks scheduled:";
                }
                              
                
                bell_png.Visibility = Visibility.Visible;
                expander.Visibility = Visibility.Visible;
                exp1text.Text = msg;
                button5.Visibility = Visibility.Visible;
            }
        }


        // SHUTDOWN NOW
        private void button2_Click(object sender, RoutedEventArgs e)
        {
            shnow.Content = "Shutting down in: ";
            Timer.Start();
            button2.Visibility = Visibility.Collapsed;
            button7.Visibility = Visibility.Visible;
            shnow.Visibility = Visibility.Visible;
            TBCountDown.Visibility = Visibility.Visible;
        }
        // CANCEL SHUTDOWN NOW
        private void button7_Click(object sender, RoutedEventArgs e)
        {
            Timer.Stop();
            time = 7;
            button7.Visibility = Visibility.Collapsed;
            button2.Visibility = Visibility.Visible;
            shnow.Visibility = Visibility.Collapsed;
            TBCountDown.Visibility = Visibility.Hidden;
        }

        //CANCEL RESTART NOW
        private void rbutton7_Click(object sender, RoutedEventArgs e)
        {
            Timer.Stop();
            time = 7;
            rbutton7.Visibility = Visibility.Collapsed;
            rbutton2.Visibility = Visibility.Visible;
            rshnow.Visibility = Visibility.Collapsed;
            TBCountDown.Visibility = Visibility.Collapsed;
        }
        // RESTART NOW
        private void rbutton2_Click(object sender, RoutedEventArgs e)
        {
            rshnow.Content = "Restarting in: ";
            Timer.Start();
            rbutton2.Visibility = Visibility.Collapsed;
            rbutton7.Visibility = Visibility.Visible;
            rshnow.Visibility = Visibility.Visible;
            TBCountDown.Visibility = Visibility.Visible;
        }

        //RESTART OK
        private void rbutton1_Click(object sender, RoutedEventArgs e)
        {
            string text = rtextBox.Text;
            string text1 = rtextBox1.Text;
            string units = rcomboBoxtime.Text;
        

            string msg = "";

            //timed shutdown
            if (rcheckBox0.IsChecked.Value)
            {
                //AT
                if (rradio1.IsChecked.Value)
                {
                    if (text != "HH:MM" && text != "")
                    {
                        msg = "Computer will restart at: " + text;                        
                    }
                }
                //IN
                else
                {
                    if (text1 != "")
                    {
                        msg = "Computer will restart in: " + text1 + " " + units;                        
                    }
                }
            }
            if (msg != "")
            {
                msg = msg + ".";
                MessageBox.Show(msg);
                //task added
                tasks++;
                if (tasks == 1)
                {
                    header.Content = "You have " + tasks + " task scheduled:";
                }
                else if (tasks > 1)
                {
                    header.Content = "You have " + tasks + " tasks scheduled:";
                }
                bell_png.Visibility = Visibility.Visible;
                exp2text.Text = msg;
                expander1.Visibility = Visibility.Visible;
                button5.Visibility = Visibility.Visible;
            }
        }

        //nofitication pressed
        private void bell_png_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            myTabControl.SelectedIndex = 4;
        }
        
        //shutdown task cancel
        private void button5_Click(object sender, RoutedEventArgs e)
        {
            tasks--;
            if (tasks == 1)
            {
                header.Content = "You have " + tasks + " task scheduled:";
            }
            else if (tasks == 0)
            {
                header.Content = "You have no tasks scheduled.";
                bell_png.Visibility = Visibility.Collapsed;
            }

            expander.Visibility = Visibility.Collapsed;
            exp1text.Text = "";
            
        }

        //restart task cancel
        private void button6_Click_1(object sender, RoutedEventArgs e)
        {
            tasks--;
            if (tasks == 1)
            {
                header.Content = "You have " + tasks + " task scheduled:";
            }
            else if (tasks == 0)
            {
                header.Content = "You have no tasks scheduled.";
                bell_png.Visibility = Visibility.Collapsed;
            }

            expander1.Visibility = Visibility.Collapsed;
            exp2text.Text = "";
        }




        //sleep task cancel 
        private void button20_Click(object sender, RoutedEventArgs e)
        {
            tasks--;
            if (tasks == 1)
            {
                header.Content = "You have " + tasks + " task scheduled:";
            }
            else if (tasks == 0)
            {
                header.Content = "You have no tasks scheduled.";
                bell_png.Visibility = Visibility.Collapsed;
            }

            expander2.Visibility = Visibility.Collapsed;
            exp3text.Text = "";
        }

 
        //SLEEP OK BUTTON
        private void sbutton1_Click(object sender, RoutedEventArgs e)
        {
            string text = stextBox.Text;
            string text1 = stextBox1.Text;
            string units = scomboBoxtime.Text;


            string msg = "";

            //timed shutdown
            if (scheckBox0.IsChecked.Value)
            {
                //AT
                if (sradio1.IsChecked.Value)
                {
                    if (text != "HH:MM" && text != "")
                    {
                        msg = "Computer will be put to sleep at: " + text;
                    }
                }
                //IN
                else
                {
                    if (text1 != "")
                    {
                        msg = "Computer will be put to sleep in: " + text1 + " " + units;
                    }
                }
            }
            if (checkBox1.IsChecked.Value && msg != "")
            {
                //sleep + hibernate
                msg = msg + " (hibernation)";
            }
            if (msg != "")
            {
                msg = msg + ".";
                MessageBox.Show(msg);
                //task added
                tasks++;
                if (tasks == 1)
                {
                    header.Content = "You have " + tasks + " task scheduled:";
                }
                else if (tasks > 1)
                {
                    header.Content = "You have " + tasks + " tasks scheduled:";
                }
                bell_png.Visibility = Visibility.Visible;
                exp3text.Text = msg;
                expander2.Visibility = Visibility.Visible;
                button20.Visibility = Visibility.Visible;
            }
        }

        // LOCK OK
        private void lbutton1_Click(object sender, RoutedEventArgs e)
        {
            string text = ltextBox.Text;
            string text1 = ltextBox1.Text;
            string units = lcomboBoxtime.Text;
            string text2 = ltextBox2.Text;
            string units2 = lcomboBoxtime.Text;

            string msg = "";

            //timed shutdown
            if (lcheckBox0.IsChecked.Value)
            {
                //AT
                if (lradio1.IsChecked.Value)
                {
                    if (text != "HH:MM" && text != "")
                    {
                        msg = "Computer will be locked at: " + text;
                    }
                }
                //IN
                else if(lradio2.IsChecked.Value)
                {
                    if (text1 != "")
                    {
                        msg = "Computer will be locked in: " + text1 + " " + units;
                    }
                }
                else if(lradio3.IsChecked.Value)
                {
                    if (text2 != "")
                    {
                        msg = "Computer will be locked after " + text2 + " " + units2 + " of inactivity";
                    }
                }
            }
            if (lheckBox1.IsChecked.Value && msg != "")
            {
                //sleep + hibernate
                msg = msg + ", logged out as well";
            }
            if (msg != "")
            {
                msg = msg + ".";
                MessageBox.Show(msg);
                //task added
                tasks++;
                if (tasks == 1)
                {
                    header.Content = "You have " + tasks + " task scheduled:";
                }
                else if (tasks > 1)
                {
                    header.Content = "You have " + tasks + " tasks scheduled:";
                }
                bell_png.Visibility = Visibility.Visible;
                exp4text.Text = msg;
                expander3.Visibility = Visibility.Visible;
                button30.Visibility = Visibility.Visible;
            }
        }

        // lock task cancel
        private void button30_Click(object sender, RoutedEventArgs e)
        {
            tasks--;
            if (tasks == 1)
            {
                header.Content = "You have " + tasks + " task scheduled:";
            }
            else if (tasks == 0)
            {
                header.Content = "You have no tasks scheduled.";
                bell_png.Visibility = Visibility.Collapsed;
            }

            expander3.Visibility = Visibility.Collapsed;
            exp4text.Text = "";
        }

        // sleep now
        private void button3_Click(object sender, RoutedEventArgs e)
        {
            Timer.Start();
            sleepin_label.Visibility = Visibility.Visible;
            sleepin.Visibility = Visibility.Visible;
            button3.Visibility = Visibility.Collapsed;
            button8.Visibility = Visibility.Visible;
        }
        //sleep now cancel
        private void button8_Click(object sender, RoutedEventArgs e)
        {
            Timer.Stop();
            time = 7;
            button8.Visibility = Visibility.Collapsed;
            sleepin_label.Visibility = Visibility.Collapsed;
            sleepin.Visibility = Visibility.Collapsed;
            button3.Visibility = Visibility.Visible;

        }
        //hibernate now cancel
        private void button9_Click(object sender, RoutedEventArgs e)
        {
            Timer.Stop();
            time = 7;
            button9.Visibility = Visibility.Collapsed;
            hibin.Visibility = Visibility.Collapsed;
            hibins.Visibility = Visibility.Collapsed;
            button4.Visibility = Visibility.Visible;
        }
        //hibernate now
        private void button4_Click(object sender, RoutedEventArgs e)
        {
            Timer.Start();
            hibin.Visibility = Visibility.Visible;
            hibins.Visibility = Visibility.Visible;
            button4.Visibility = Visibility.Collapsed;
            button9.Visibility = Visibility.Visible;
        }

        //lock now cancel
        private void lcknow_Click(object sender, RoutedEventArgs e)
        {
            Timer.Stop();
            time = 7;
            lcknow.Visibility = Visibility.Collapsed;
            label.Visibility = Visibility.Collapsed;
            lckin.Visibility = Visibility.Collapsed;
            lbutton3.Visibility = Visibility.Visible;
        }
        //lock now
        private void lbutton3_Click(object sender, RoutedEventArgs e)
        {
            Timer.Start();
            label.Visibility = Visibility.Visible;
            lckin.Visibility = Visibility.Visible;
            lbutton3.Visibility = Visibility.Collapsed;
            lcknow.Visibility = Visibility.Visible;
        }


        //logout cancel
        private void button10_Click(object sender, RoutedEventArgs e)
        {
            Timer.Stop();
            time = 7;
            button10.Visibility = Visibility.Collapsed;
            label2.Visibility = Visibility.Collapsed;
            logoutin.Visibility = Visibility.Collapsed;
            lbutton4.Visibility = Visibility.Visible;
        }
        //logout now
        private void lbutton4_Click(object sender, RoutedEventArgs e)
        {
            Timer.Start();
            label2.Visibility = Visibility.Visible;
            logoutin.Visibility = Visibility.Visible;
            lbutton4.Visibility = Visibility.Collapsed;
            button10.Visibility = Visibility.Visible;
        }
    }
}
