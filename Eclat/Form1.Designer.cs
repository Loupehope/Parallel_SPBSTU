
namespace Eclat
{
    partial class Form1
    {
        /// <summary>
        /// Обязательная переменная конструктора.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Освободить все используемые ресурсы.
        /// </summary>
        /// <param name="disposing">истинно, если управляемый ресурс должен быть удален; иначе ложно.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Код, автоматически созданный конструктором форм Windows

        /// <summary>
        /// Требуемый метод для поддержки конструктора — не изменяйте 
        /// содержимое этого метода с помощью редактора кода.
        /// </summary>
        private void InitializeComponent()
        {
            this.rowsNumberTextBox = new System.Windows.Forms.TextBox();
            this.checkBox1 = new System.Windows.Forms.CheckBox();
            this.startButton = new System.Windows.Forms.Button();
            this.stopButton = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.currentResultTextBox = new System.Windows.Forms.TextBox();
            this.previousResultTextBox = new System.Windows.Forms.TextBox();
            this.progressTextBox = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.rowsAvailableLabel = new System.Windows.Forms.Label();
            this.supportTextBox = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // rowsNumberTextBox
            // 
            this.rowsNumberTextBox.Location = new System.Drawing.Point(53, 332);
            this.rowsNumberTextBox.Name = "rowsNumberTextBox";
            this.rowsNumberTextBox.Size = new System.Drawing.Size(395, 31);
            this.rowsNumberTextBox.TabIndex = 0;
            this.rowsNumberTextBox.TextChanged += new System.EventHandler(this.rowsNumberTextBox_TextChanged);
            // 
            // checkBox1
            // 
            this.checkBox1.AutoSize = true;
            this.checkBox1.Location = new System.Drawing.Point(53, 470);
            this.checkBox1.Name = "checkBox1";
            this.checkBox1.Size = new System.Drawing.Size(281, 29);
            this.checkBox1.TabIndex = 3;
            this.checkBox1.Text = "Включить паралеллизм";
            this.checkBox1.UseVisualStyleBackColor = true;
            this.checkBox1.CheckedChanged += new System.EventHandler(this.checkBox1_CheckedChanged);
            // 
            // startButton
            // 
            this.startButton.Location = new System.Drawing.Point(53, 529);
            this.startButton.Name = "startButton";
            this.startButton.Size = new System.Drawing.Size(395, 77);
            this.startButton.TabIndex = 6;
            this.startButton.Text = "Старт";
            this.startButton.UseVisualStyleBackColor = true;
            this.startButton.Click += new System.EventHandler(this.startButton_Click);
            // 
            // stopButton
            // 
            this.stopButton.Location = new System.Drawing.Point(53, 629);
            this.stopButton.Name = "stopButton";
            this.stopButton.Size = new System.Drawing.Size(395, 77);
            this.stopButton.TabIndex = 7;
            this.stopButton.Text = "Стоп";
            this.stopButton.UseVisualStyleBackColor = true;
            this.stopButton.Click += new System.EventHandler(this.stopButton_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(48, 273);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(314, 25);
            this.label1.TabIndex = 8;
            this.label1.Text = "Число обрабатываемых строк";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(524, 57);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(207, 25);
            this.label2.TabIndex = 9;
            this.label2.Text = "Текущий результат";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(1016, 57);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(214, 25);
            this.label3.TabIndex = 10;
            this.label3.Text = "Прошлый результат";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(53, 57);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(105, 25);
            this.label4.TabIndex = 11;
            this.label4.Text = "Прогресс";
            // 
            // currentResultTextBox
            // 
            this.currentResultTextBox.BackColor = System.Drawing.SystemColors.ButtonHighlight;
            this.currentResultTextBox.Location = new System.Drawing.Point(529, 103);
            this.currentResultTextBox.Multiline = true;
            this.currentResultTextBox.Name = "currentResultTextBox";
            this.currentResultTextBox.ReadOnly = true;
            this.currentResultTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.currentResultTextBox.Size = new System.Drawing.Size(436, 603);
            this.currentResultTextBox.TabIndex = 12;
            // 
            // previousResultTextBox
            // 
            this.previousResultTextBox.BackColor = System.Drawing.SystemColors.ButtonHighlight;
            this.previousResultTextBox.Location = new System.Drawing.Point(1021, 103);
            this.previousResultTextBox.Multiline = true;
            this.previousResultTextBox.Name = "previousResultTextBox";
            this.previousResultTextBox.ReadOnly = true;
            this.previousResultTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.previousResultTextBox.Size = new System.Drawing.Size(436, 603);
            this.previousResultTextBox.TabIndex = 13;
            // 
            // progressTextBox
            // 
            this.progressTextBox.BackColor = System.Drawing.SystemColors.ButtonHighlight;
            this.progressTextBox.Location = new System.Drawing.Point(53, 103);
            this.progressTextBox.Multiline = true;
            this.progressTextBox.Name = "progressTextBox";
            this.progressTextBox.ReadOnly = true;
            this.progressTextBox.Size = new System.Drawing.Size(395, 135);
            this.progressTextBox.TabIndex = 14;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(49, 304);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(114, 25);
            this.label5.TabIndex = 15;
            this.label5.Text = "Доступно:";
            // 
            // rowsAvailableLabel
            // 
            this.rowsAvailableLabel.AutoSize = true;
            this.rowsAvailableLabel.Location = new System.Drawing.Point(170, 302);
            this.rowsAvailableLabel.Name = "rowsAvailableLabel";
            this.rowsAvailableLabel.Size = new System.Drawing.Size(24, 25);
            this.rowsAvailableLabel.TabIndex = 16;
            this.rowsAvailableLabel.Text = "0";
            // 
            // supportTextBox
            // 
            this.supportTextBox.Location = new System.Drawing.Point(53, 415);
            this.supportTextBox.Name = "supportTextBox";
            this.supportTextBox.Size = new System.Drawing.Size(395, 31);
            this.supportTextBox.TabIndex = 17;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(53, 387);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(87, 25);
            this.label6.TabIndex = 18;
            this.label6.Text = "Support";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(12F, 25F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoSize = true;
            this.ClientSize = new System.Drawing.Size(1524, 734);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.supportTextBox);
            this.Controls.Add(this.rowsAvailableLabel);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.progressTextBox);
            this.Controls.Add(this.previousResultTextBox);
            this.Controls.Add(this.currentResultTextBox);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.stopButton);
            this.Controls.Add(this.startButton);
            this.Controls.Add(this.checkBox1);
            this.Controls.Add(this.rowsNumberTextBox);
            this.MaximumSize = new System.Drawing.Size(1550, 805);
            this.MinimumSize = new System.Drawing.Size(1550, 805);
            this.Name = "Form1";
            this.Text = "Eclat";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox rowsNumberTextBox;
        private System.Windows.Forms.CheckBox checkBox1;
        private System.Windows.Forms.Button startButton;
        private System.Windows.Forms.Button stopButton;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox currentResultTextBox;
        private System.Windows.Forms.TextBox previousResultTextBox;
        private System.Windows.Forms.TextBox progressTextBox;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label rowsAvailableLabel;
        private System.Windows.Forms.TextBox supportTextBox;
        private System.Windows.Forms.Label label6;
    }
}

