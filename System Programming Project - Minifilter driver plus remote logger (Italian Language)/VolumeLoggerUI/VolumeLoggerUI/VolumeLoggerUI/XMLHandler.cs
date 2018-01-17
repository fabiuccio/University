using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using System.Xml.Schema;
using System.IO;

namespace VolumeLoggerUI
{
    class XMLHandler
    {
        private XMLHandler() { }

        public static void WriteXML(string filename, string[] values)
        {
            try
            {
                /* Writing DTD */
                StreamWriter Tex = new StreamWriter(filename.Substring(0, filename.LastIndexOf('\\')+1) + "VLdtd.dtd");
                Tex.WriteLine("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
                Tex.WriteLine("<!ELEMENT VolumeLog (LogRecord)*>");
                Tex.WriteLine("<!ELEMENT LogRecord (HostIp, (HostName)?, SequenceNumber, ProcessName, Timestamp, FileName, Action, (ActionDetails)?, (Notes)?)>");
                Tex.WriteLine("<!ELEMENT HostName EMPTY>");
                Tex.WriteLine("<!ATTLIST HostName");
                Tex.WriteLine("          value CDATA #REQUIRED>");
                Tex.WriteLine("<!ELEMENT HostIp EMPTY>");
                Tex.WriteLine("<!ATTLIST HostIp");
                Tex.WriteLine("          value CDATA #REQUIRED>");
                Tex.WriteLine("<!ELEMENT SequenceNumber EMPTY>");
                Tex.WriteLine("<!ATTLIST SequenceNumber");
                Tex.WriteLine("          value CDATA #REQUIRED>");
                Tex.WriteLine("<!ELEMENT ProcessName EMPTY>");
                Tex.WriteLine("<!ATTLIST ProcessName");
                Tex.WriteLine("          value CDATA #REQUIRED>");
                Tex.WriteLine("<!ELEMENT Timestamp EMPTY>");
                Tex.WriteLine("<!ATTLIST Timestamp");
                Tex.WriteLine("          value CDATA #REQUIRED>");
                Tex.WriteLine("<!ELEMENT FileName EMPTY>");
                Tex.WriteLine("<!ATTLIST FileName");
                Tex.WriteLine("          value CDATA #REQUIRED>");
                Tex.WriteLine("<!ELEMENT Action EMPTY>");
                Tex.WriteLine("<!ATTLIST Action");
                Tex.WriteLine("          value CDATA #REQUIRED>");
                Tex.WriteLine("<!ELEMENT ActionDetails EMPTY>");
                Tex.WriteLine("<!ATTLIST ActionDetails");
                Tex.WriteLine("          value CDATA #REQUIRED>");
                Tex.WriteLine("<!ELEMENT Notes EMPTY>");
                Tex.WriteLine("<!ATTLIST Notes");
                Tex.WriteLine("          value CDATA #REQUIRED>");
                Tex.Close();
                /* End DTD file creation*/

                XmlDocument xmlDoc = new XmlDocument();

                if (!File.Exists(filename))
                {
                    //if file is not found, create a new xml file
                    XmlTextWriter xmlWriter = new XmlTextWriter(filename, System.Text.Encoding.UTF8);
                    xmlWriter.Formatting = Formatting.Indented;
                    xmlWriter.WriteProcessingInstruction("xml", "version='1.0' encoding='UTF-8'");
                    xmlWriter.WriteDocType("VolumeLog", null, "VLdtd.dtd", null);

                    //<!DOCTYPE Order SYSTEM "vldtd.dtd">

                    xmlWriter.WriteStartElement("VolumeLog");
                    //If WriteProcessingInstruction is used as above,
                    //Do not use WriteEndElement() here
                    //xmlWriter.WriteEndElement();
                    //it will cause the <Root></Root> to be <Root />
                    xmlWriter.Close();
                    xmlDoc.Load(filename);
                }else
                    xmlDoc.Load(filename);

                XmlNode root = xmlDoc.DocumentElement;
                XmlElement childNode = xmlDoc.CreateElement("LogRecord");

                XmlElement cchildNodeIp = xmlDoc.CreateElement("HostIp");
                    cchildNodeIp.SetAttribute("value", secureXml(values[1]));
                XmlElement cchildNode1 = xmlDoc.CreateElement("SequenceNumber");
                    cchildNode1.SetAttribute("value", secureXml(values[2]));
                XmlElement cchildNode2 = xmlDoc.CreateElement("ProcessName");
                    cchildNode2.SetAttribute("value", secureXml(values[3]));  
                XmlElement cchildNode3 = xmlDoc.CreateElement("Timestamp");
                    cchildNode3.SetAttribute("value", secureXml(values[4]));   
                XmlElement cchildNode4 = xmlDoc.CreateElement("FileName");
                    cchildNode4.SetAttribute("value", secureXml(values[5]));   
                XmlElement cchildNode5 = xmlDoc.CreateElement("Action");
                    cchildNode5.SetAttribute("value", secureXml(values[6]));
               
                root.AppendChild(childNode);
                childNode.AppendChild(cchildNodeIp);
                if (values[0] != null && values[0].Length > 0)
                {
                    XmlElement cchildNode0 = xmlDoc.CreateElement("HostName");
                    cchildNode0.SetAttribute("value", secureXml(values[0]));
                    childNode.AppendChild(cchildNode0);
                }
                childNode.AppendChild(cchildNode1);
                childNode.AppendChild(cchildNode2);
                childNode.AppendChild(cchildNode3);
                childNode.AppendChild(cchildNode4);
                childNode.AppendChild(cchildNode5);

                if (values[7]!= null && values[7].Length>0)
                {
                    XmlElement cchildNode6 = xmlDoc.CreateElement("ActionDetails");
                    cchildNode6.SetAttribute("value", secureXml(values[7]));
                    childNode.AppendChild(cchildNode6);
                }
                if (values[8] != null && values[8].Length > 0)
                {
                    XmlElement cchildNode7 = xmlDoc.CreateElement("Notes");
                    cchildNode7.SetAttribute("value", secureXml(values[8]));
                    childNode.AppendChild(cchildNode7);
                } 
                xmlDoc.Save(filename);

            }
            catch(Exception ex)
            {
                Console.Write(ex.ToString());
            }
        }

        public static void ReadXML(string filename, MainForm.AddRecordDelegate myAR)
        {
            XmlReader reader = null;
            try
            {
                XmlDocument xmlDoc = new XmlDocument();

                // Set the validation settings.
                XmlReaderSettings settings = new XmlReaderSettings();
                settings.ProhibitDtd = false;
                settings.ConformanceLevel = ConformanceLevel.Document;
                settings.ValidationType = ValidationType.DTD;
                settings.ValidationEventHandler += new ValidationEventHandler(ValidationCallback);

                // Create the XmlReader object.
                reader = XmlReader.Create(filename, settings);
                // Parse the file. 
                xmlDoc.Load(reader);
                XmlNode root = xmlDoc.DocumentElement;
                foreach (XmlElement child in root.ChildNodes)
                {
                    string[] values = new string[9];

                    XmlNodeList cchildNodeIp = child.GetElementsByTagName("HostIp");
                    values[1]=cchildNodeIp[0].Attributes[0].Value;

                    XmlNodeList cchildNode0 = child.GetElementsByTagName("HostName");
                    if (cchildNode0.Count != 0)
                    {
                        values[0] = cchildNode0[0].Attributes[0].Value;
                    }
                    else values[0] = null;

                    XmlNodeList cchildNode1 = child.GetElementsByTagName("SequenceNumber");
                    values[2]=cchildNode1[0].Attributes[0].Value;

                    XmlNodeList cchildNode2 = child.GetElementsByTagName("ProcessName");
                    values[3]=cchildNode2[0].Attributes[0].Value;

                    XmlNodeList cchildNode3 = child.GetElementsByTagName("Timestamp");
                    values[4]=cchildNode3[0].Attributes[0].Value;

                    XmlNodeList cchildNode4 = child.GetElementsByTagName("FileName");
                    values[5]=cchildNode4[0].Attributes[0].Value;

                    XmlNodeList cchildNode5 = child.GetElementsByTagName("Action");
                    values[6]=cchildNode5[0].Attributes[0].Value;

                    XmlNodeList cchildNode6 = child.GetElementsByTagName("ActionDetails");
                    if (cchildNode6.Count != 0)
                    {
                        values[7] = cchildNode6[0].Attributes[0].Value;
                    }
                    else values[7] = "";
                    XmlNodeList cchildNode7 = child.GetElementsByTagName("Notes");
                    if (cchildNode7.Count != 0)
                    {
                        values[8] = cchildNode7[0].Attributes[0].Value;
                    }else values[8] = "";

                    myAR(values);
                }

            }
            catch (Exception ex)
            {
                System.Windows.Forms.MessageBox.Show("Error while parsing: file is corrupted.\n"+
                                                     "Content won't be retrieved.\nError:" + ex.ToString()); 
            }
            finally
            {
                if (reader != null)
                    reader.Close();                
            }
        }
        // Display the validation error.
        static void ValidationCallback(object sender, ValidationEventArgs e)
        {
            System.Windows.Forms.MessageBox.Show("Error/Warning while parsing: maybe XML file is corrupted.\nContent will be retrieved until possible.\nError:" + e.Message);
            switch (e.Severity)
            {
                case XmlSeverityType.Error:
                    break;
                case XmlSeverityType.Warning:
                    break;
            }
        }

        private static string secureXml(string secureTxt)
        {
            string temp="";
            foreach (char c in secureTxt.ToCharArray())
            {
                if (c < 32) temp += '?';
                else temp += c;
            }
            return temp;
        }
    }
}
