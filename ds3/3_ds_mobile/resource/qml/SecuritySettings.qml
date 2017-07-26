
import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

import "header"

Page
{
    // Header
    header: SettingsItemHeader
    {
        id: header
        anchors
        {
            top: parent.top;
            left: parent.left;
            right: parent.right;
        }
        enabled: !wizard_active || adding_client;
        visible: !wizard_active || adding_client;
        height: wizard_active && !adding_client? 0 : header_size;
        width: parent.width;
    }

    // Background
    ColumnLayout
    {
        id: group_settings_root

        // Variables
        property string enabled_color: Group.enabled ? blue : light_grey

        // Margins
        anchors.topMargin: margin_height
        anchors.bottomMargin: margin_height
        anchors.leftMargin: margin_width
        anchors.rightMargin: margin_width

        anchors.fill: parent

        // Signals
        signal save();

        onSave:
        {
            Chat.clear();

            Group.enabled = group_checkbox.checked;
            Group.entropy = entropy_text_area.text;
            Group.cipher = cipher_text_combo_box.currentText;
            Group.key_size = parseInt(key_size_combo_box.currentText);
            Group.hash = hash_combo_box.currentText;
            Group.iterations = parseInt(iterations_text_field.text);
            Group.salt = salt_text_area.text;

            Client.restart();
			App.save();

            Group.idChanged();
            Toast.show("Settings saved");
        }

        Component.onCompleted:
        {
            // Font
            cipher_text_combo_box.popup.font.pixelSize = box_font_size;
            key_size_combo_box.popup.font.pixelSize = box_font_size;
            hash_combo_box.popup.font.pixelSize = box_font_size;

            // Enabled
            group_checkbox.checked = Group.enabled;
            entropy_text.enabled = Group.enabled;
            entropy_text_area.enabled = Group.enabled;
            id_group_crc.enabled = Group.enabled;

            cipher_text_combo_box.enabled = Group.enabled;
            key_size_combo_box.enabled = Group.enabled;
            hash_combo_box.enabled = Group.enabled;
            iterations_text_field.enabled = Group.enabled;
            salt_text_area.enabled = Group.enabled;

            // Color
            group_settings_root.enabled_color = Group.enabled ? blue : light_grey;
            id_group_crc.color = group_settings_root.enabled_color;
            entropy_text.color = group_settings_root.enabled_color;
            cipher_text.color = group_settings_root.enabled_color;
            key_size_text.color = group_settings_root.enabled_color;
            hash_text.color = group_settings_root.enabled_color;
            iterations_text.color = group_settings_root.enabled_color;
            salt_text.color = group_settings_root.enabled_color;

            // Default
            entropy_text_area.text = Group.entropy;
            cipher_text_combo_box.currentIndex = cipher_text_combo_box.find(Group.cipher);
            key_size_combo_box.currentIndex = key_size_combo_box.find(Group.key_size.toString());
            hash_combo_box.currentIndex = hash_combo_box.find(Group.hash);
            iterations_text_field.text = Group.iterations.toString();
            salt_text_area.text = Group.salt;
        }

        // Background Mouse Area
        MouseArea
        {
           anchors.fill:parent;

           onPressed:
           {
               forceActiveFocus();
           }
        }

        // Checkbox + Entropy Row
        Rectangle
        {
            id: id_enabled_row
            anchors
            {
                top: parent.top;
                left: parent.left;
                right: parent.right;
                topMargin: margin_height / 2;
            }
            height: screen_height / 32;
            width: parent.width;

            // CheckBox Widget
            CheckBox
            {
                id: group_checkbox;

                anchors
                {
                    left: parent.left;
                    leftMargin: margin_width * 0.3;
                    verticalCenter: parent.verticalCenter;
                }

                scale: scale_ratio;
                width: box_font_size;

                onCheckStateChanged:
                {
					if(wizard_active)
						Group.enabled = checked;

					id_group_crc.enabled = checked;
					entropy_text.enabled = checked;
					entropy_text_area.enabled = checked;
					cipher_text_combo_box.enabled = checked;
					key_size_combo_box.enabled = checked;
					hash_combo_box.enabled = checked;
					iterations_text_field.enabled = checked;
					salt_text_area.enabled = checked;

					group_settings_root.enabled_color = checked ? blue : light_grey;
					id_group_crc.color = group_settings_root.enabled_color;
					entropy_text.color = group_settings_root.enabled_color;
					cipher_text.color = group_settings_root.enabled_color;
					key_size_text.color = group_settings_root.enabled_color;
					hash_text.color = group_settings_root.enabled_color;
					iterations_text.color = group_settings_root.enabled_color;
					salt_text.color = group_settings_root.enabled_color;			
                }
            }

            // CheckBox Text
            Text
            {
                id: id_enabled_text;
                anchors
                {
                    left: group_checkbox.right;
                    leftMargin: margin_width;
                    verticalCenter: parent.verticalCenter;
                }
                color: blue;
                font.pixelSize: box_font_size;
                text: "Private Group";
                width: (parent.width - group_checkbox.width) / 2;

                // CheckBox MouseArea
                MouseArea
                {
                    anchors.fill: parent;
                    onClicked:
                    {
                        group_checkbox.checked = !group_checkbox.checked;
                    }
                }
            }

            // Id
            Text
            {
                id: id_group_crc;
                anchors
                {
                    left: id_enabled_text.right;
                    right: parent.right;
                    verticalCenter: parent.verticalCenter;
                }
                color: blue;
                horizontalAlignment: Text.AlignRight;
                font.pixelSize: box_font_size;
                height: box_font_size;
                text: Group.id;
                width: (parent.width - group_checkbox.width) / 2;
            }
        }

        // Row 0
        Item
        {
            id: setting_row0
            anchors
            {
                top: id_enabled_row.bottom;
                left: parent.left;
                right: parent.right;
                topMargin: margin_height * 1.5;
            }

            height: screen_height / 6;
            width: parent.width;

            // Entropy Text
            Text
            {
                id: entropy_text
                anchors
                {
                    left: parent.left;
                    top: parent.top;
                    right: parent.right;
                }

                text: qsTr("Passphrase")
                horizontalAlignment: Text.AlignLeft
                font.pixelSize: box_font_size
            }

            // Entropy Text Area
            TextArea
            {
                id: entropy_text_area
                anchors
                {
                    top: entropy_text.bottom;
                    left: parent.left;
                    right: parent.right;
                    topMargin: margin_height / 2;
                }
                height: combo_box_height * 2;
				inputMethodHints: Qt.ImhNoPredictiveText;
                placeholderText: qsTr("None");
                font.pixelSize: box_font_size;
				leftPadding: margin_width * 0.75;
                wrapMode: Text.WrapAnywhere;
                horizontalAlignment: Text.AlignLeft;
                selectionColor: light_blue;
                selectedTextColor: white;

                background: Rectangle
                {
                    anchors.fill: parent;
                    border.color: border_color;
                    border.width: line_size;
                    z: -3;
                }

                onTextChanged:
                {
                    if(wizard_active)
                        Group.entropy = text;
                }
            }
        }

        // Setting Row 1
        Item
        {
            id: setting_row1;
            anchors
            {
                top: setting_row0.bottom;
                left: parent.left;
                right: parent.right;
                topMargin: margin_height * 2;
            }

            width: parent.width
            height: screen_height / 8;

            // Left Column: Cipher
            Item
            {
                id: left;
                anchors
                {
                    top: parent.top;
                    left: parent.left;
                    bottom: parent.bottom;
                }

                height: screen_height / 8;
                width: parent.width * (2/5);

                // Cipher Text
                Text
                {
                    id: cipher_text;
                    anchors
                    {
                        top: parent.top;
                        left: parent.left;
                    }

                    text: qsTr("Cipher");
                    font.pixelSize: box_font_size;
                }

                // Cipher ComboBox
                ComboBox
                {
                    id: cipher_text_combo_box;
                    anchors
                    {
                        top: cipher_text.bottom;
                        left: parent.left;
                        right: parent.right;
                        topMargin: margin_height / 2;
                    }

                    // Down Arrow
                    indicator: Canvas
                    {
                        id: id_cipher_canvas
                        x: parent.width - width - parent.rightPadding
                        y: parent.topPadding + (parent.availableHeight - height) / 2
                        width: margin_width
                        height: margin_width * 0.75
                        contextType: "2d"

                        onPaint:
                        {
                            var context = id_cipher_canvas.getContext("2d");
                            context.reset();
                            context.moveTo(0, 0);
                            context.lineTo(width, 0);
                            context.lineTo(width / 2, height);
                            context.closePath();
                            context.fillStyle = parent.pressed ? blue : grey;
                            context.fill();
                        }
                    }
                    onPressedChanged: id_cipher_canvas.requestPaint()

                    height: combo_box_height;
                    width: combo_box_width;
                    font.pixelSize: box_font_size;
                    model: ["aes", "mars", "rc6", "serpent", "twofish"];

                    onCurrentTextChanged:
                    {
                        if(wizard_active)
                            Group.cipher = currentText;
                    }

                    onModelChanged:
                    {
                        currentIndex = find(Group.cipher);
                    }
                }
            }

            // Right Column: Key Size
            Item
            {
                id: right;
                anchors
                {
                    top: parent.top;
                    right: parent.right;
                    bottom: parent.bottom;
                }

                width: parent.width * (2/5);
                height:screen_height / 8;

                // Key Size Text
                Text
                {
                    id: key_size_text;
                    anchors
                    {
                        top: parent.top;
                        left: parent.left;

                    }

                    text: qsTr("Key Size");
                    font.pixelSize: box_font_size;
                }

                // Key Size ComboBox
                ComboBox
                {
                    id: key_size_combo_box;
                    anchors
                    {
                        top: key_size_text.bottom;
                        right: parent.right;
                        left: parent.left;
                        topMargin: margin_height / 2;
                    }

                    currentIndex: 2;
                    height: combo_box_height;

                    // Down Arrow
                    indicator: Canvas
                    {
                        id: id_key_canvas
                        x: parent.width - width - parent.rightPadding
                        y: parent.topPadding + (parent.availableHeight - height) / 2
                        width: margin_width
                        height: margin_width * 0.75
                        contextType: "2d"

                        onPaint:
                        {
                            var context = id_key_canvas.getContext("2d");
                            context.reset();
                            context.moveTo(0, 0);
                            context.lineTo(width, 0);
                            context.lineTo(width / 2, height);
                            context.closePath();
                            context.fillStyle = parent.pressed ? blue : grey;
                            context.fill();
                        }
                    }
                    onPressedChanged: id_key_canvas.requestPaint();

                    width: combo_box_width;
                    font.pixelSize: box_font_size;
                    model: ["128", "192", "256"];

                    onCurrentTextChanged:
                    {
                        if(wizard_active)
                            Group.key_size = parseInt(currentText);
                    }

                    onModelChanged:
                    {
                        currentIndex = find(Group.key_size.toString());
                    }
                }
            }
        }

        // Setting Row 2
        Item
        {
            id: setting_row2;
            anchors
            {
                top: setting_row1.bottom;
                left: parent.left;
                right: parent.right;
                topMargin: margin_height;
            }

            width: parent.width;
            height: screen_height / 8;

            // Hash
            Item
            {
                id: left1;
                anchors
                {
                    top: parent.top;
                    left: parent.left;
                    bottom: parent.bottom;
                }

                height:screen_height / 8;
                width: parent.width * (2/5);

                // Hash Text
                Text
                {
                    id: hash_text;
                    anchors
                    {
                        top: parent.top;
                        left: parent.left;
                    }

                    text: qsTr("Hash");
                    font.pixelSize: box_font_size;
                }

                // Hash ComboBox
                ComboBox
                {
                    id: hash_combo_box;
                    anchors
                    {
                        top: hash_text.bottom;
                        left: parent.left;
                        right: parent.right;
                        topMargin: margin_height / 2;
                    }

                    currentIndex: 3;
                    height: combo_box_height;

                    // Down Arrow
                    indicator: Canvas
                    {
                        id: id_hash_canvas
                        x: parent.width - width - parent.rightPadding
                        y: parent.topPadding + (parent.availableHeight - height) / 2
                        width: margin_width
                        height: margin_width * 0.75
                        contextType: "2d"

                        onPaint:
                        {
                            var context = id_hash_canvas.getContext("2d");
                            context.reset();
                            context.moveTo(0, 0);
                            context.lineTo(width, 0);
                            context.lineTo(width / 2, height);
                            context.closePath();
                            context.fillStyle = parent.pressed ? blue : grey;
                            context.fill();
                        }
                    }
                    onPressedChanged: id_hash_canvas.requestPaint();
                    width: combo_box_width;
                    font.pixelSize: box_font_size;
                    model: [ "md5", "sha1", "sha224", "sha256", "sha384", "sha512"];

                    onCurrentTextChanged:
                    {
                        if(wizard_active)
                            Group.hash = currentText;
                    }

                    onModelChanged:
                    {
                        currentIndex = find(Group.hash);
                    }
                }
            }

            // Iterations
            Item
            {
                id: right1;
                anchors
                {
                    top: parent.top;
                    right: parent.right;
                    bottom: parent.bottom;
                }

                height:screen_height / 8;
                width: parent.width * (2/5);

                // Iterations Text
                Text
                {
                    id: iterations_text;
                    anchors.left: parent.left;
                    text: qsTr("Iterations");
                    font.pixelSize: box_font_size;
                }

                // Iterations Text Field
                TextField
                {
                    id: iterations_text_field;
                    anchors
                    {
                        top: iterations_text.bottom;
                        left: parent.left;
                        right: parent.right;
                        topMargin: margin_height / 2;
                    }

                    color: grey;
                    validator: IntValidator {bottom: 1; top: 2147483647}
                    height: combo_box_height;
                    width: combo_box_width;
                    font.pixelSize: box_font_size;
                    horizontalAlignment: Text.AlignLeft;
					leftPadding: margin_width * 0.75;
                    inputMethodHints: Qt.ImhDigitsOnly;
                    selectionColor: light_blue;
                    selectedTextColor: white;

                    background: Rectangle
                    {
                        anchors.fill:parent;
                        border.color: border_color;
                        border.width: line_size;
                        z: -3;
                    }

                    onTextChanged:
                    {
                        if(wizard_active)
                            Group.iterations = parseInt(text);
                    }
                }
            }
        }

        // Setting Row 4
        Item
        {
            id: setting_row4
            anchors
            {
                top: setting_row2.bottom;
                left: parent.left;
                right: parent.right;
                topMargin: margin_height;
            }

            height: screen_height / 7;
            width: parent.width

            // Salt Text
            Text
            {
                id: salt_text
                anchors
                {
                    left: parent.left;
                    right: parent.right;
                }

                text: qsTr("Salt")
                horizontalAlignment: Text.AlignLeft
                font.pixelSize: box_font_size
            }

            // Salt Text Area
            TextArea
            {
                id: salt_text_area
                anchors
                {
                    top: salt_text.bottom;
                    left: parent.left;
                    right: parent.right;
                    topMargin: margin_height / 2;
                }
                height: combo_box_height * 2;
				inputMethodHints: Qt.ImhNoPredictiveText;
                placeholderText: qsTr("None");
                font.pixelSize: box_font_size;
				leftPadding: margin_width * 0.75;
                wrapMode: Text.WrapAnywhere;
                horizontalAlignment: Text.AlignLeft;
                selectionColor: light_blue;
                selectedTextColor: white;

                background: Rectangle
                {
                    anchors.fill: parent;
                    border.color: border_color;
                    border.width: line_size;
                    z: -3;
                }

                onTextChanged:
                {
                    if(wizard_active)
                        Group.salt = text;
                }
            }
        }
    }
}
