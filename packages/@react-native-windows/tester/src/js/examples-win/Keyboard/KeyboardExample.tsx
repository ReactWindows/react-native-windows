/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import React from 'react';
import {Text, Picker, TouchableHighlight, View, ViewStyle} from 'react-native';

class TabStopExample extends React.Component {
  public render() {
    const itemStyle: ViewStyle = {
      width: 120,
      height: 50,
      backgroundColor: 'aqua',
      justifyContent: 'center',
      alignItems: 'center',
    };
    const pickerStyle: ViewStyle = {width: 120, height: 50};

    return (
      <View>
        <Text>No tab index, this item will be tabbed to last</Text>
        <TouchableHighlight style={itemStyle}>
          <Text>tabIndex default</Text>
        </TouchableHighlight>

        <Text>
          These 3 items should tab in the order of first, last, middle:
        </Text>
        <View
          style={{
            flex: 1,
            flexDirection: 'row',
            justifyContent: 'space-between',
          }}>
          <TouchableHighlight style={itemStyle} {...{tabIndex: 1}}>
            <Text>tabIndex 1</Text>
          </TouchableHighlight>
          <TouchableHighlight style={itemStyle} {...{tabIndex: 3}}>
            <Text>tabIndex 3</Text>
          </TouchableHighlight>
          <TouchableHighlight style={itemStyle} {...{tabIndex: 2}}>
            <Text>tabIndex 2</Text>
          </TouchableHighlight>
        </View>

        <Text>
          Controls like Picker should also do the same tab in the order of
          first, last, middle:
        </Text>
        <View
          style={{
            flex: 1,
            flexDirection: 'row',
            justifyContent: 'space-between',
          }}>
          <Picker style={pickerStyle} {...{tabIndex: 11}}>
            <Picker.Item label="tabIndex 11" />
          </Picker>
          <Picker style={pickerStyle} {...{tabIndex: 13}}>
            <Picker.Item label="tabIndex 13" />
          </Picker>
          <Picker style={pickerStyle} {...{tabIndex: 12}}>
            <Picker.Item label="tabIndex 12" />
          </Picker>
        </View>
      </View>
    );
  }
}

export const displayName = (_undefined?: string) => {};
export const title = 'Keyboard';
export const description = 'Usage of keyboard properties.';
export const examples = [
  {
    title: 'Tabstops',
    render: function(): JSX.Element {
      return <TabStopExample />;
    },
  },
];
