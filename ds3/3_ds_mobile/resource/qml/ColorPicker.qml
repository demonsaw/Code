import QtQuick 2.7
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.0
import QtGraphicalEffects 1.0

import "controls"
import "colorpicker"
import "colorpicker/ColorUtils.js" as ColorUtils

Item
{
    id: colorpicker_root
    anchors.fill: parent
    focus: true

    // Variables
    property vector4d colorRGBA: ColorUtils.hsva2rgba(colorpicker_root.colorHSVA)
    property vector4d colorHSVA: Qt.vector4d(1, 0, 1, 1)
    property vector4d colorRGBA_bright: ColorUtils.hsva2rgba(Qt.vector4d(colorHSVA.x, colorHSVA.y, 1, colorHSVA.w))
    property vector4d initColor: Qt.vector4d(0,0,0,0);

    // Signals
    signal accepted;
    signal updateTextColor;

    // Update color and text color
    onAccepted:
    {
        var color = Qt.rgba(colorRGBA.x, colorRGBA.y, colorRGBA.z, colorRGBA.w);

        // Update color
        name_text.background.color = color;
        if(wizard_active)
        {
            start_button_background.color = color;
            id_wizard_statusbar.color = color;
        }
        selected_color = color;

        // Update text color
        updateTextColor();
    }

    // Update Text color
    onUpdateTextColor:
    {
        if(((colorRGBA.x * 255 * 0.299 + colorRGBA.y * 255 * 0.587 + colorRGBA.z * 255 * 0.114) / 255) <= 0.5)
            client_text_color = white;
        else
            client_text_color = grey;
        name_text.color = client_text_color;
        if(wizard_active)
            start_button_text.color = client_text_color;
    }

    Component.onCompleted:
    {
        // Initial Color
        initColor = Client.get_color_hsva();
        wheel.updateHS(initColor.x, initColor.y, initColor.z, initColor.w);
        slider1.value = initColor.z;

        // Initial Text Color
        name_text.color = Client.text_color;
        client_text_color = Client.text_color;
        if(wizard_active)
        {
            start_button_text.color = Client.text_color;
            start_button_background.color = Client.color;
        }

        colorpicker_root.accepted();
    }

    RowLayout
    {
        id: row1
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top:parent.top
        width: parent.width
        height: parent.height * 0.80

        // Color wheel
        Wheel
        {
            id: wheel
            anchors.fill: parent;
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumWidth: 50
            Layout.minimumHeight: 50

            hue: colorHSVA.x
            saturation: colorHSVA.y

            // Update Color
            onUpdateHS:
            {
                colorHSVA = Qt.vector4d(hueSignal,saturationSignal, colorHSVA.z, colorHSVA.w)
                if(wizard_active)
                    wizard_root.colorModified(colorRGBA);
            }

            // Accepted Signal
            onAccepted:
            {
                colorpicker_root.accepted()
            }
        }
    }

    CustomSlider
    {
        id: slider1;
        rotation: 180;
        anchors
        {
            top: row1.bottom;
            topMargin: margin_height * 2;
            left: parent.left;
            right: parent.right;
        }
        height: parent.height / 18;

        // Update wheel color
        onValueChanged:
        {
            colorHSVA.z = value
            if(wizard_active)
                wizard_root.colorModified(colorRGBA);

            colorpicker_root.state = '';
            colorpicker_root.accepted() ;
        }
    }
}
