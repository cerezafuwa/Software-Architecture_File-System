package Document;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.util.Vector;

import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JScrollPane;
import javax.swing.JTextField;


/**
 * @author hubeini
 *         组合模式
 *         MyFile为Document的子类【叶子节点】
 *         实现有关文件的操作
 */

public class MyFile extends MyDocument {
    Vector<String> bufferString = new Vector(10);
    FileViewPanel fileView = new FileViewPanel();

    UndoableTextArea textArea;

    int bufferIndex;
    int fontSize;
    String text = "";

    JMenuBar menuBar;
    JFrame frame;
    JPanel panel;

    JFrame replaceFrame;
    JMenuItem undo;
    JMenuItem redo;


    class FileViewPanel extends JPanel {
        void setViewPanel() {
            viewPanel.setBackground(Color.white);
            viewPanel.setBounds(15, 5, 70, 70);
            viewPanel.addMouseListener(fileMouseListener);
            viewImg = new ImageIcon("resource/file.png");
            JLabel imgLabel = new JLabel(viewImg);
            viewPanel.add(imgLabel);
            add(viewPanel);
        }

        void setNamePanel() {
            nameField.setHorizontalAlignment(JTextField.CENTER);
            nameField.setEditable(false);
            nameField.setBackground(Color.white);
            nameField.setBounds(10, 80, 80, 20);
            add(nameField);
        }

        FileViewPanel() {
            setLayout(null);
            setBackground(Color.white);
            setPreferredSize(new Dimension(100, 100));
            setViewPanel();
            setNamePanel();

        }

        MouseListener fileMouseListener = new MouseListener() {

            public void mouseClicked(MouseEvent e) {

                if (e.getButton() == MouseEvent.BUTTON3) {
                    JPopupMenu menu = new JPopupMenu();
                    JMenuItem openMenu = new JMenuItem("打开");
                    openMenu.addActionListener(openMenuListener);
                    menu.add(openMenu);

                    JMenuItem editMenu = new JMenuItem("编辑");
                    editMenu.addActionListener(openMenuListener);
                    menu.add(editMenu);

                    JMenuItem deleteMenu = new JMenuItem("删除");
                    deleteMenu.addActionListener(deleteMenuListener);
                    menu.add(deleteMenu);

                    JMenuItem resetNameMenu = new JMenuItem("重命名");
                    resetNameMenu.addActionListener(resetNameMenuListener);
                    menu.add(resetNameMenu);
                    if (!isHide) {
                        JMenuItem hideMenu = new JMenuItem("隐藏文件");
                        hideMenu.addActionListener(hideMenuListener);
                        menu.add(hideMenu);
                    } else {
                        JMenuItem showMenu = new JMenuItem("显示文件");
                        showMenu.addActionListener(showMenuListener);
                        menu.add(showMenu);
                    }

                    JMenuItem propertyMenu = new JMenuItem("属性");
                    propertyMenu.addActionListener(propertyMenuListener);
                    menu.add(propertyMenu);

                    menu.show(e.getComponent(), e.getX(), e.getY());
                }
                if (e.getButton() == MouseEvent.BUTTON1 && e.getClickCount() == 2) {
                    viewPanel.setBackground(Color.white);
                    open();
                }
            }

            public void mouseEntered(MouseEvent arg0) {
                viewPanel.setBackground(Color.lightGray);
            }

            public void mouseExited(MouseEvent arg0) {
                viewPanel.setBackground(Color.white);
            }

            public void mousePressed(MouseEvent arg0) {
            }

            public void mouseReleased(MouseEvent e) {
            }

            ActionListener openMenuListener = new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    open();
                }
            };
            ActionListener deleteMenuListener = new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    delete(true);
                }
            };

            ActionListener hideMenuListener = new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    hideFile(true);
                }
            };

            ActionListener showMenuListener = new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    hideFile(false);
                }
            };

            ActionListener resetNameMenuListener = new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    resetName();
                }
            };

            ActionListener propertyMenuListener = new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    showProperty();
                }
            };
        };
    }

        void setMenuBar(UndoableTextArea textArea){
            menuBar = new JMenuBar();
            menuBar.setBounds(0, 0, 500, 20);

            JMenu file = new JMenu("文件");
            JMenuItem save = new JMenuItem("保存文件");
            save.addActionListener(saveButtonListener);
            JMenuItem exit = new JMenuItem("退出");
            exit.addActionListener(exitButtonListener);
            file.add(exit);
            file.add(save);

            //撤销和重做

            JMenu edit = new JMenu("编辑");
            undo=new UndoCommand(textArea);
            undo.addActionListener(undoButtonListener);

            redo = new RedoCommand(textArea);
            redo.addActionListener(redoButtonListener);
            edit.add(undo);
            edit.add(redo);

            JMenu setting = new JMenu("设置");
            JMenuItem font = new JMenuItem("字体设置");
            font.addActionListener(fontButtonListener);
            setting.add(font);
            menuBar.add(file);
            menuBar.add(setting);
            menuBar.add(edit);
            panel.add(menuBar);
        }

    void setTextArea() {
        textArea = new UndoableTextArea();
        textArea.setBackground(Color.white);
        textArea.addKeyListener(setTextListener);
        JScrollPane scroll = new JScrollPane(textArea);
        scroll.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
        scroll.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED);
        panel.add(scroll);
    }


    //创建记事本
    public void createTxt() {
        setMenuBar(textArea);
        setTextArea();

        frame = new JFrame();
        frame.setJMenuBar(menuBar);
        frame.add(panel);
        frame.setSize(400, 500);
        frame.setLocationRelativeTo(null);
        frame.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
        frame.addWindowListener(closeListener);
    }

    public MyFile(ContentPanel father) {
        fatherContentPanel = father;
        panel = new JPanel();
        panel.setBackground(Color.white);
        panel.setLayout(new GridLayout(1, 1));

        createTxt();

        create();
    }

    public MyFile(Block block, ContentPanel father) {
        fatherContentPanel = father;
        fatherContentPanel.fileList.add(this);

        panel = new JPanel();
        panel.setBackground(Color.white);
        panel.setLayout(new GridLayout(1, 1));

        createTxt();
        getProperty(block);
        nameField.setText(name);
        text = block.data;
        if (text == null) text = "";
        whoAmI = "文件";
    }
    
    ///////////抽象函数接口的具体实现////////////////
    public void create() {
        setCreateTime();
        whoAmI = "文件";
        nameField.setText("新建文件");
        resetName(true);
    }

    /////////打开
    public void open() {
        if (frame.isShowing()) {
            frame.requestFocus();
            return;
        }

        setVisitTime();
        block.setProperty(this);
        frame.show();
        frame.setTitle("记事本——" + name);
        textArea.setText(text);

        undo.setEnabled(false);
        redo.setEnabled(false);
        bufferString.clear();
        bufferIndex = 0;
        bufferString.add(text);
    }

    //////////删除
    public boolean delete(boolean isRootPanel) {
        if (frame.isShowing()) {
            frame.requestFocus();
            JOptionPane.showMessageDialog(null, "文件正在被使用", "删除错误", JOptionPane.ERROR_MESSAGE, null);
            return false;
        } else {
            if (isRootPanel) {
                int option = JOptionPane.showConfirmDialog(
                        null, "删除文件？",
                        "是否删除文件", JOptionPane.YES_NO_OPTION,
                        JOptionPane.WARNING_MESSAGE, null);
                if (option == JOptionPane.NO_OPTION) return false;
            }

            Disk.fat.deleteBlock(this);
            fatherContentPanel.delete(this);

            Block fatherBlock;
            if (fatherContentPanel.getFolder() == null) fatherBlock = Disk.block[0];
            else fatherBlock = fatherContentPanel.getFolder().block;
            fatherBlock.setData(fatherContentPanel);


            return true;
        }
    }

    //////////设置字体
    void setTextAreaFontSize(int selectSize) {
        fontSize = selectSize;
        textArea.setFont(new Font(textArea.getFont().getFontName(), textArea.getFont().getStyle(), fontSize));
    }

    //////添加入缓冲，给撤消使用
    void addBuffer() {

        if (bufferString.get(bufferIndex).equals(textArea.getText())) return;
        for (int i = bufferString.size() - 1; i > bufferIndex; i--) {
            bufferString.remove(i);
        }
        if (bufferString.size() < 10) {
            bufferIndex++;
            bufferString.add(textArea.getText());
        } else {
            for (int i = 0; i < 9; i++) {
                bufferString.set(i, bufferString.get(i + 1));
            }
            bufferString.set(9, textArea.getText());
        }
        if (bufferIndex != 0) undo.setEnabled(true);
        else undo.setEnabled(false);
        if (bufferIndex != bufferString.size() - 1) redo.setEnabled(true);
        else redo.setEnabled(false);
    }

    ////////退出
    void fileExit() {
        if (replaceFrame != null) replaceFrame.dispose();
        if (!text.equals(textArea.getText())) {
            int option = JOptionPane.showConfirmDialog(
                    null, "文件已修改，是否保存？",
                    "是否保存文件", JOptionPane.YES_NO_CANCEL_OPTION,
                    JOptionPane.WARNING_MESSAGE, null);
            switch (option) {
                case JOptionPane.YES_OPTION:
                    text = textArea.getText();
                    block.setData(text);
                    setModifiTime();
                    frame.dispose();
                    break;
                case JOptionPane.NO_OPTION:
                    frame.dispose();
                    break;
                case JOptionPane.CANCEL_OPTION:
                    break;
            }
        } else frame.dispose();

        block.setProperty(this);
    }

    ///////隐藏
    void hideFile(boolean hide) {
        isHide = hide;
        fatherContentPanel.refresh();
        block.setProperty(this);
    }

    //////建仓是否有重复名字
    boolean checkName(String str) {
        for (int i = 0; i < fatherContentPanel.fileList.size(); i++) {
            if (fatherContentPanel.fileList.get(i) != this && fatherContentPanel.fileList.get(i).name.equals(str))
                return false;
        }
        return true;
    }

    ////////////自动调整重命名
    void resetName(boolean isAutomic) {
        String newName = nameField.getText();
        while (!newName.isEmpty() && newName.charAt(newName.length() - 1) == ' ')
            newName = newName.substring(0, newName.length() - 1);
        while (!newName.isEmpty() && newName.charAt(0) == ' ') newName = newName.substring(1);

        if (newName.length() > 10) newName = newName.substring(0, 10);

        if (newName.length() == 0) {
            nameField.setText(name);
            return;
        }

        if (!checkName(newName)) {
            int i = 0;
            while (!checkName(newName + "(" + new Integer(++i).toString() + ')')) ;
            if (!isAutomic) {
                JOptionPane.showMessageDialog(null, "系统已帮你修复重命名", "重命名出错", JOptionPane.YES_OPTION, null);
            }
            newName = newName + "(" + new Integer(i).toString() + ')';
        }
        name = newName;
        nameField.setText(name);

        if (!isAutomic) {
            setModifiTime();
            block.setProperty(this);
        }
    }

    public void resetName() {
        nameField.setEditable(true);
        nameField.addFocusListener(nameFieldFocusListener);
        nameField.addActionListener(nameFieldActionListener);
    }

    ///获取路径
    public String getAddress() {
        return fatherAddress + name;
    }


    //////////////////////////////监听器///////////////////////////

    /////////////////////////////命名////////////////////////////
    FocusListener nameFieldFocusListener = new FocusListener() {
        public void focusGained(FocusEvent arg0) {
        }

        public void focusLost(FocusEvent e) {
            nameField.setEditable(false);
            resetName(false);
        }
    };

    ActionListener nameFieldActionListener = new ActionListener() {
        public void actionPerformed(ActionEvent arg0) {
            fatherContentPanel.requestFocus();
        }
    };

    //////////////////////////////关闭///////////////////////////////
    WindowAdapter closeListener = new WindowAdapter() {
        public void windowClosing(WindowEvent e) {
            fileExit();
        }
    };

    //////////////////////////////保存////////////////////
    ActionListener saveButtonListener = new ActionListener() {
        public void actionPerformed(ActionEvent e) {
            text = textArea.getText();
            block.setData(text);
        }
    };

    //////////////////////////////撤消/////////////////////////

	ActionListener undoButtonListener = new ActionListener()
	{
		public void actionPerformed(ActionEvent e)
		{
			textArea.setText(bufferString.get(--bufferIndex));
			if (bufferIndex!=0) undo.setEnabled(true);
			else undo.setEnabled(false);
			if (bufferIndex!=bufferString.size()-1) redo.setEnabled(true);
			else redo.setEnabled(false);
            //Command cmd = (Command)e.getSource();
            //cmd.execute();
        }
	};

    //////////////////////////////重做//////////////////////////
    ActionListener redoButtonListener = new ActionListener() {
        public void actionPerformed(ActionEvent e) {
            textArea.setText(bufferString.get(++bufferIndex));
            if (bufferIndex != 0) undo.setEnabled(true);
            else undo.setEnabled(false);
            if (bufferIndex != bufferString.size() - 1) redo.setEnabled(true);
            else redo.setEnabled(false);
        }
    };
    /////////////////////////////退出////////////////////////////
    ActionListener exitButtonListener = new ActionListener() {
        public void actionPerformed(ActionEvent e) {
            fileExit();
        }
    };

    /////////////////////////////设置/////////////////////////////
    ActionListener fontButtonListener = new ActionListener() {
        public void actionPerformed(ActionEvent e) {
            int oldFontSize = textArea.getFont().getSize();
            Integer[] newFontSize = new Integer[26];
            for (int i = 0; i < 16; i++) {
                newFontSize[i] = new Integer(i + 10);
            }
            Object selectSize = JOptionPane.showInputDialog(null, "选择字体大小", "字体设置",
                    JOptionPane.INFORMATION_MESSAGE, null, newFontSize, newFontSize[oldFontSize - 10]);
            if (selectSize != null) setTextAreaFontSize((Integer) selectSize);
        }
    };

    ///////////////////////////修改文档//////////////////////
    KeyListener setTextListener = new KeyListener() {
        public void keyPressed(KeyEvent arg0) {
        }

        public void keyReleased(KeyEvent arg0) {
            addBuffer();
        }

        public void keyTyped(KeyEvent arg0) {
        }

    };
}
