package com.home.vpestret.xmlparserexample;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import java.io.IOException;
import java.io.StringReader;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;
import org.xmlpull.v1.XmlPullParserFactory;
import android.view.View;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    public void buttonClick(View v) {
        TextView textView = (TextView)findViewById(R.id.editText);

        String console_out = "";
        try {
            XmlPullParserFactory factory = XmlPullParserFactory.newInstance();
            factory.setNamespaceAware(true);
            XmlPullParser xpp = factory.newPullParser();

            xpp.setInput(new StringReader("<foo>Hello World!</foo>"));
            int eventType = xpp.getEventType();
            while (eventType != XmlPullParser.END_DOCUMENT) {
                if (eventType == XmlPullParser.START_DOCUMENT) {
                    console_out += "Start document\n";
                } else if (eventType == XmlPullParser.START_TAG) {
                    console_out += "Start tag " + xpp.getName() + "\n";
                } else if (eventType == XmlPullParser.END_TAG) {
                    console_out += "End tag " + xpp.getName() + "\n";
                } else if (eventType == XmlPullParser.TEXT) {
                    console_out += "Text " + xpp.getText() + "\n";
                }
                eventType = xpp.next();
            }
            console_out += "End document\n";
        } catch (XmlPullParserException e) {

        } catch (IOException e) {
        }

        textView.setText(console_out, TextView.BufferType.EDITABLE);
    }
}
