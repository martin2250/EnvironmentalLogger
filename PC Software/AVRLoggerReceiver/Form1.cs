using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;
using System.Windows.Forms.DataVisualization.Charting;
using System.IO;

namespace AVRLoggerReceiver
{
	public partial class Form1 : Form
	{

		static string PointPair(DateTime t, double value)
		{
			return string.Format("[Date.UTC({0}, {1}, {2}, {3}, {4}), {5}]", t.Year, t.Month - 1, t.Day, t.Hour, t.Minute, value.ToString().Replace(',', '.'));
		}

		List<DataPoint> Measurements = new List<DataPoint>();

		Series TemperatureS, HumidityS, BatteryS;

		receiverSettings Settings = receiverSettings.Default;

		public Form1()
		{
			InitializeComponent();

			try { numericUpDownGraphSamples.Value = Settings.GraphPoints; }
			catch { }

			List<string> ports = new List<string>(SerialPort.GetPortNames());

			comboBoxPort.Items.AddRange(ports.ToArray());

			if (ports.IndexOf(Settings.Port) != -1)
				comboBoxPort.SelectedIndex = ports.IndexOf(Settings.Port);

			TemperatureS = chart1.Series[0];
			HumidityS = chart1.Series[1];
			BatteryS = chart1.Series[2];

		}

		private void comboBoxPort_DropDown(object sender, EventArgs e)
		{
			comboBoxPort.Items.Clear();
			comboBoxPort.Items.AddRange(SerialPort.GetPortNames());
		}

		private void buttonOpen_Click(object sender, EventArgs e)
		{
			serialPort.PortName = (string)comboBoxPort.SelectedItem;

			try
			{
				serialPort.Open();

				TemperatureS.Points.Clear();
				HumidityS.Points.Clear();
				BatteryS.Points.Clear();
				Measurements.Clear();
				progressBar1.Value = 0;

				PageBuffer = new byte[128];
				BufferPointer = 0;
				TotalPages = -1;
				PageCounter = 0;

				timer1.Start();

				buttonOpen.Enabled = false;
				buttonClose.Enabled = true;
				numericUpDownGraphSamples.Enabled = false;

				samplepointer = (int)numericUpDownGraphSamples.Value - 1;
			}
			catch
			{
				MessageBox.Show("Could not open Port");
			}
		}

		private void buttonSave_Click(object sender, EventArgs e)
		{
			saveFileDialog1.ShowDialog();
		}

		private void Form1_FormClosing(object sender, FormClosingEventArgs e)
		{
			Settings.GraphPoints = (int)numericUpDownGraphSamples.Value;
			Settings.Port = (string)comboBoxPort.SelectedItem;
			Settings.Save();
		}

		byte[] PageBuffer = new byte[128];
		int BufferPointer = 0;

		int TotalPages = -1;
		int PageCounter = 0;

		int samplepointer = 0;

		private void timer1_Tick(object sender, EventArgs e)
		{
			while (serialPort.BytesToRead > 0)
			{
				if (TotalPages == -1)
				{

					int i = serialPort.ReadByte();
					if (i == 0)
						continue;
					if (i == 255)
					{
						while (serialPort.BytesToRead < 2) ;
						TotalPages = serialPort.ReadByte() << 8;
						TotalPages |= serialPort.ReadByte();



						progressBar1.Maximum = TotalPages;
					}

				}
				else
				{
					BufferPointer += serialPort.Read(PageBuffer, BufferPointer, 128 - BufferPointer);
				}

				if (BufferPointer == 128)
				{
					BufferPointer = 0;

					progressBar1.Value++;
					PageCounter++;

					int Samples = PageBuffer[0];

					int Interval = PageBuffer[1] & 0x3F;

					int batraw = PageBuffer[1] >> 6;
					batraw |= (PageBuffer[2] & 0xF) << 2;

					float? Battery = batraw / 10.0F;

					int Month = PageBuffer[2] >> 4;

					int Year = PageBuffer[3] + 2000;

					int Day = PageBuffer[4];
					int Hour = PageBuffer[5];
					int Minute = PageBuffer[6];
					int Second = PageBuffer[7];

					DateTime Time;

					Time = new DateTime(Year, Month, Day, Hour, Minute, Second);

					for (int i = 0; i < Samples; i++)
					{
						float Temperature = ((int)PageBuffer[8 + i * 3]) << 4 | PageBuffer[9 + i * 3] >> 4;
						Temperature = (Temperature * 165F / 4096F) - 40;

						float Humididy = ((int)PageBuffer[9 + i * 3] & 0xF) << 8 | PageBuffer[10 + i * 3];
						Humididy = Humididy * 100 / 4096;

						Measurements.Add(new DataPoint { BatteryLevel = Battery, Temperature = Temperature, Humidity = Humididy, Time = Time });

						if (++samplepointer == (int)numericUpDownGraphSamples.Value)
						{
							TemperatureS.Points.AddXY(Time, Temperature);
							HumidityS.Points.AddXY(Time, Humididy);
							samplepointer = 0;
						}

						if (Battery.HasValue)
							BatteryS.Points.AddXY(Time, Battery);

						Battery = null;

						Time = Time.AddMinutes(Interval);
					}

				}

			}

			if (PageCounter == TotalPages)
				buttonClose_Click(null, null);
		}

		private void buttonClose_Click(object sender, EventArgs e)
		{
			timer1.Stop();
			serialPort.Close();

			buttonOpen.Enabled = true;
			buttonClose.Enabled = false;

			numericUpDownGraphSamples.Enabled = true;
		}

		private void saveFileDialog1_FileOk(object sender, CancelEventArgs e)
		{
			StreamWriter w = new StreamWriter(saveFileDialog1.FileName);

			switch (saveFileDialog1.FilterIndex)
			{
				case 1:
					w.Write(Properties.Resources.Graph_P1);

					foreach (DataPoint p in Measurements)
					{
						w.Write(PointPair(p.Time, Math.Round(p.Temperature, 2)));
						if (!p.Equals(Measurements.Last()))
							w.Write(',');
					}

					w.Write(Properties.Resources.Graph_P2);

					foreach (DataPoint p in Measurements)
					{
						w.Write(PointPair(p.Time, Math.Round(p.Humidity, 2)));
						if (!p.Equals(Measurements.Last()))
							w.Write(',');
					}

					w.Write(Properties.Resources.Graph_P3);

					foreach (DataPoint p in Measurements.Where(p => p.BatteryLevel.HasValue))
					{
						w.Write(PointPair(p.Time, Math.Round((float)p.BatteryLevel, 1)));
						w.Write(',');
					}

					w.Write(PointPair(Measurements.Last().Time, Math.Round((float)Measurements.Where(p => p.BatteryLevel.HasValue).Last().BatteryLevel, 1)));

					w.Write(Properties.Resources.Graph_P4);
					w.Write(Properties.Resources.Graph_P5);
					break;
				case 2:
					w.Write("Time;Temperature;Humidity;Battery");
					foreach (DataPoint p in Measurements)
					{
						w.WriteLine();
						w.Write(p.Time);
						w.Write(';');
						w.Write(Math.Round(p.Temperature, 2));
						w.Write(';');
						w.Write(Math.Round(p.Humidity, 2));
						w.Write(';');
						if(p.BatteryLevel.HasValue)
							w.Write(p.BatteryLevel);
					}
					break;
				default:
					MessageBox.Show("Wrong file type selected " + saveFileDialog1.FilterIndex.ToString());
					break;
			}
			w.Close();
		}
	}

	struct DataPoint
	{
		public float Temperature, Humidity;
		public float? BatteryLevel;
		public DateTime Time;
	}




}
