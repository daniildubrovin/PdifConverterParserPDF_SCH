#pragma once

#include "framework.h"
#include <PDIFException.h>

namespace CLRTEST {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Сводка для MyForm
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
			//
			//TODO: добавьте код конструктора
			//
		}

	protected:
		/// <summary>
		/// Освободить все используемые ресурсы.
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^ button_open_file;

	private: System::Windows::Forms::Button^ button_to_svg;


	private: System::Windows::Forms::TextBox^ textBox1;
	private: System::Windows::Forms::TextBox^ textBox_to_svg;
	private: System::ComponentModel::IContainer^ components;
	private: System::Windows::Forms::ContextMenuStrip^ contextMenuStrip1;
	private: System::Windows::Forms::Label^ label1;
	private: System::Windows::Forms::TextBox^ textBoxScale;



	protected:

	protected:

	private:
		/// <summary>
		/// Обязательная переменная конструктора.
		/// </summary>


		// 
		String^ selectFile;
		String^ schScale = "2.0";

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Требуемый метод для поддержки конструктора — не изменяйте 
		/// содержимое этого метода с помощью редактора кода.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->button_open_file = (gcnew System::Windows::Forms::Button());
			this->button_to_svg = (gcnew System::Windows::Forms::Button());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->textBox_to_svg = (gcnew System::Windows::Forms::TextBox());
			this->contextMenuStrip1 = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->textBoxScale = (gcnew System::Windows::Forms::TextBox());
			this->SuspendLayout();
			// 
			// button_open_file
			// 
			this->button_open_file->BackColor = System::Drawing::Color::OrangeRed;
			this->button_open_file->Font = (gcnew System::Drawing::Font(L"Impact", 36, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->button_open_file->ForeColor = System::Drawing::SystemColors::ButtonHighlight;
			this->button_open_file->Location = System::Drawing::Point(219, 28);
			this->button_open_file->Name = L"button_open_file";
			this->button_open_file->Size = System::Drawing::Size(555, 107);
			this->button_open_file->TabIndex = 0;
			this->button_open_file->Text = L"Выбрать файл";
			this->button_open_file->UseVisualStyleBackColor = false;
			this->button_open_file->Click += gcnew System::EventHandler(this, &MyForm::button_open_file_Click);
			// 
			// button_to_svg
			// 
			this->button_to_svg->BackColor = System::Drawing::Color::Lime;
			this->button_to_svg->Font = (gcnew System::Drawing::Font(L"Impact", 27.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->button_to_svg->ForeColor = System::Drawing::SystemColors::ControlLightLight;
			this->button_to_svg->Location = System::Drawing::Point(219, 258);
			this->button_to_svg->Name = L"button_to_svg";
			this->button_to_svg->Size = System::Drawing::Size(283, 136);
			this->button_to_svg->TabIndex = 2;
			this->button_to_svg->Text = L"TO SVG";
			this->button_to_svg->UseVisualStyleBackColor = false;
			this->button_to_svg->Click += gcnew System::EventHandler(this, &MyForm::button_to_svg_Click);
			// 
			// textBox1
			// 
			this->textBox1->BackColor = System::Drawing::SystemColors::ButtonHighlight;
			this->textBox1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->textBox1->Location = System::Drawing::Point(219, 163);
			this->textBox1->MinimumSize = System::Drawing::Size(4, 40);
			this->textBox1->Multiline = true;
			this->textBox1->Name = L"textBox1";
			this->textBox1->ReadOnly = true;
			this->textBox1->Size = System::Drawing::Size(555, 64);
			this->textBox1->TabIndex = 3;
			// 
			// textBox_to_svg
			// 
			this->textBox_to_svg->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->textBox_to_svg->Location = System::Drawing::Point(74, 427);
			this->textBox_to_svg->MaximumSize = System::Drawing::Size(1000, 1000);
			this->textBox_to_svg->Multiline = true;
			this->textBox_to_svg->Name = L"textBox_to_svg";
			this->textBox_to_svg->Size = System::Drawing::Size(841, 219);
			this->textBox_to_svg->TabIndex = 4;
			this->textBox_to_svg->TextChanged += gcnew System::EventHandler(this, &MyForm::textBox_to_svg_TextChanged);
			// 
			// contextMenuStrip1
			// 
			this->contextMenuStrip1->Name = L"contextMenuStrip1";
			this->contextMenuStrip1->Size = System::Drawing::Size(61, 4);
			// 
			// label1
			// 
			this->label1->BackColor = System::Drawing::SystemColors::ButtonHighlight;
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 20.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label1->ForeColor = System::Drawing::SystemColors::Desktop;
			this->label1->Location = System::Drawing::Point(527, 258);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(247, 49);
			this->label1->TabIndex = 5;
			this->label1->Text = L"SCALE";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// textBoxScale
			// 
			this->textBoxScale->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->textBoxScale->Location = System::Drawing::Point(524, 325);
			this->textBoxScale->Multiline = true;
			this->textBoxScale->Name = L"textBoxScale";
			this->textBoxScale->Size = System::Drawing::Size(250, 41);
			this->textBoxScale->TabIndex = 6;
			this->textBoxScale->Text = L"2.0";
			this->textBoxScale->TextChanged += gcnew System::EventHandler(this, &MyForm::textBoxScale_TextChanged);
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::ButtonHighlight;
			this->ClientSize = System::Drawing::Size(1004, 681);
			this->Controls->Add(this->textBoxScale);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->textBox_to_svg);
			this->Controls->Add(this->textBox1);
			this->Controls->Add(this->button_to_svg);
			this->Controls->Add(this->button_open_file);
			this->Name = L"MyForm";
			this->Text = L"Converter";
			this->Load += gcnew System::EventHandler(this, &MyForm::MyForm_Load);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void MyForm_Load(System::Object^ sender, System::EventArgs^ e) {

	}

	private: System::Void button_open_file_Click(System::Object^ sender, System::EventArgs^ e) {
		OpenFileDialog^ openDlg = gcnew OpenFileDialog();
		openDlg->Filter = "Files(*.PDF;*.SCH;*.PCB;*.SYM;*.PLC;*.PRT;*.BAK)|*.PDF;*.SCH;*.PCB;*.SYM;*.PLC;*.PRT;*.BAK|All files (*.*)|*.*";
		if (openDlg->ShowDialog() == System::Windows::Forms::DialogResult::OK)
		{
			String^ fileName = openDlg->FileName;
			textBox1->Text = fileName;
			selectFile = fileName;
		}
	}

	/*private: System::Void button_to_pdif_Click(System::Object^ sender, System::EventArgs^ e) 
	{
		FolderBrowserDialog^ openDlg = gcnew FolderBrowserDialog();
		if (openDlg->ShowDialog() == System::Windows::Forms::DialogResult::OK)
		{
			String^ fileName = openDlg->SelectedPath;
			textBox_to_pdif->Text = fileName;

			if (selectFile != nullptr || !selectFile->Contains(".pdf"))
			{
				std::string file;
				convertString(selectFile, file);
				PDIFParser pdifParser(file);
				PDIFType pdif = pdifParser.parse();
			}
		

		}
	}*/
private: System::Void textBox3_TextChanged(System::Object^ sender, System::EventArgs^ e) {
}
private: System::Void button_to_svg_Click(System::Object^ sender, System::EventArgs^ e) 
{
	this->textBox_to_svg->ForeColor = System::Drawing::Color::Black;
	this->textBox_to_svg->Text = "";

	FolderBrowserDialog^ openDlg = gcnew FolderBrowserDialog();
	if (openDlg->ShowDialog() == System::Windows::Forms::DialogResult::OK)
	{
		String^ dirName = openDlg->SelectedPath;
		textBox_to_svg->Text = dirName;

		if(selectFile != nullptr) 
		{
			std::string svg, pathToFile;
			convertString(selectFile, pathToFile);

			std::string format = pathToFile.substr(pathToFile.length() - 3, 3);
			if (format == "PDF" || format == "pdf")
			{
				try
				{
					PDIFParser parser(pathToFile);
					convertString(dirName, svg);

					size_t slesh = pathToFile.find_last_of('\\') + 1;
					std::string fileName = pathToFile.substr(slesh, pathToFile.length() - slesh - 3) + "svg";

					svg += '\\' + fileName;
					PDIFType pd = parser.parse();				
					pd.toSVG(svg);
				}
				catch (const PDIFException& e)
				{
					std::string log = "an error was detected when parsing the file " + pathToFile + " :\n" + e.what();
					String^ error = gcnew String(log.c_str());
					textBox_to_svg->Text = error;
					this->textBox_to_svg->ForeColor = System::Drawing::Color::Red;
				}

			
			}
			else
			{
				std::string scale;
				convertString(schScale, scale);

				SCHParser parser(pathToFile, stod(scale));

				convertString(dirName, svg);

				size_t slesh = pathToFile.find_last_of('\\') + 1;
				std::string fileName = pathToFile.substr(slesh, pathToFile.length() - slesh - 3) + "svg";

				svg += '\\' + fileName;

				try
				{
					parser.to_svg(svg);
				}
				catch (const SCHParser::Exception& e)
				{
					std::string log = "an error was detected when parsing the file " + pathToFile + " :\n" + 
					 std::to_string(e.file_pos) + ": " + e.msg;

					String^ error = gcnew String(log.c_str());
					textBox_to_svg->Text = error;
					this->textBox_to_svg->ForeColor = System::Drawing::Color::Red;
				}
				

				parser.clear_memory();
			}

			
		}

		
	}
}
private: System::Void textBox_to_svg_TextChanged(System::Object^ sender, System::EventArgs^ e) {
}
private: System::Void textBoxScale_TextChanged(System::Object^ sender, System::EventArgs^ e) {
	schScale = this->textBoxScale->Text;
}
};
}
