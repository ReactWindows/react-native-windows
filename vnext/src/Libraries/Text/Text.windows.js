/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * #7149 should be strict local
 * @flow
 * @format
 */

import DeprecatedTextPropTypes from '../DeprecatedPropTypes/DeprecatedTextPropTypes';
import * as PressabilityDebug from '../Pressability/PressabilityDebug';
import usePressability from '../Pressability/usePressability';
import StyleSheet from '../StyleSheet/StyleSheet';
import processColor from '../StyleSheet/processColor';
import TextAncestor from './TextAncestor';
import {NativeText, NativeVirtualText} from './TextNativeComponent';
import {type TextProps} from './TextProps';
import * as React from 'react';
import {useContext, useMemo, useState} from 'react';
const View = require('../Components/View/View');
import {type TextStyleProp, type ViewStyleProp} from '../StyleSheet/StyleSheet'; // [Windows]
/**
 * Text is the fundamental component for displaying text.
 *
 * @see https://reactnative.dev/docs/text.html
 */
const Text: React.AbstractComponent<
  TextProps,
  React.ElementRef<typeof NativeText | typeof NativeVirtualText>,
> = React.forwardRef((props: TextProps, forwardedRef) => {
  const {
    accessible,
    allowFontScaling,
    ellipsizeMode,
    onLongPress,
    onPress,
    onPressIn,
    onPressOut,
    onResponderGrant,
    onResponderMove,
    onResponderRelease,
    onResponderTerminate,
    onResponderTerminationRequest,
    onStartShouldSetResponder,
    pressRetentionOffset,
    suppressHighlighting,
    ...restProps
  } = props;

  const [isHighlighted, setHighlighted] = useState(false);

  const isPressable =
    (onPress != null ||
      onLongPress != null ||
      onStartShouldSetResponder != null) &&
    restProps.disabled !== true;

  const initialized = useLazyInitialization(isPressable);
  const config = useMemo(
    () =>
      initialized
        ? {
            disabled: !isPressable,
            pressRectOffset: pressRetentionOffset,
            onLongPress,
            onPress,
            onPressIn(event) {
              setHighlighted(!suppressHighlighting);
              onPressIn?.(event);
            },
            onPressOut(event) {
              setHighlighted(false);
              onPressOut?.(event);
            },
            onResponderTerminationRequest_DEPRECATED: onResponderTerminationRequest,
            onStartShouldSetResponder_DEPRECATED: onStartShouldSetResponder,
          }
        : null,
    [
      initialized,
      isPressable,
      pressRetentionOffset,
      onLongPress,
      onPress,
      onPressIn,
      onPressOut,
      onResponderTerminationRequest,
      onStartShouldSetResponder,
      suppressHighlighting,
    ],
  );

  const eventHandlers = usePressability(config);
  const eventHandlersForText = useMemo(
    () =>
      eventHandlers == null
        ? null
        : {
            onResponderGrant(event) {
              eventHandlers.onResponderGrant(event);
              if (onResponderGrant != null) {
                onResponderGrant(event);
              }
            },
            onResponderMove(event) {
              eventHandlers.onResponderMove(event);
              if (onResponderMove != null) {
                onResponderMove(event);
              }
            },
            onResponderRelease(event) {
              eventHandlers.onResponderRelease(event);
              if (onResponderRelease != null) {
                onResponderRelease(event);
              }
            },
            onResponderTerminate(event) {
              eventHandlers.onResponderTerminate(event);
              if (onResponderTerminate != null) {
                onResponderTerminate(event);
              }
            },
            onResponderTerminationRequest:
              eventHandlers.onResponderTerminationRequest,
            onStartShouldSetResponder: eventHandlers.onStartShouldSetResponder,
          },
    [
      eventHandlers,
      onResponderGrant,
      onResponderMove,
      onResponderRelease,
      onResponderTerminate,
    ],
  );

  // TODO: Move this processing to the view configuration.
  const selectionColor =
    restProps.selectionColor == null
      ? null
      : processColor(restProps.selectionColor);

  let style = restProps.style;
  if (__DEV__) {
    if (PressabilityDebug.isEnabled() && onPress != null) {
      style = StyleSheet.compose(restProps.style, {
        color: 'magenta',
      });
    }
  }

  const hasTextAncestor = useContext(TextAncestor);

  //let styleProps: ViewStyleProp = (props.style: any);
  if (hasTextAncestor) {
    return (
      <NativeVirtualText
        {...restProps}
        {...eventHandlersForText}
        isHighlighted={isHighlighted}
        selectionColor={selectionColor}
        style={style}
        ref={forwardedRef}
      />
    );
  } else {
    let styleProps: ViewStyleProp = (restProps.style: any);
    if (
      styleProps &&
      styleProps.borderColor &&
      (styleProps.borderWidth ||
        styleProps.borderBottomWidth ||
        styleProps.borderEndWidth ||
        styleProps.borderLeftWidth ||
        styleProps.borderRightWidth ||
        styleProps.borderStartWidth ||
        styleProps.borderTopWidth)
    ) {
      let textStyleProps = Array.isArray(styleProps)
        ? StyleSheet.flatten(styleProps)
        : styleProps;
      let {
        margin,
        marginBottom,
        marginEnd,
        marginHorizontal,
        marginLeft,
        marginRight,
        marginStart,
        marginTop,
        marginVertical,
        padding,
        paddingBottom,
        paddingEnd,
        paddingHorizontal,
        paddingLeft,
        paddingRight,
        paddingStart,
        paddingTop,
        paddingVertical,
        ...rest
      } = textStyleProps != null ? textStyleProps : {};

      let {style, ...textPropsLessStyle} = props;
      return (
        <View style={styleProps}>
          <TextAncestor.Provider value={true}>
            <NativeText
              {...textPropsLessStyle}
              {...eventHandlersForText}
              accessible={accessible !== false}
              allowFontScaling={allowFontScaling !== false}
              ellipsizeMode={ellipsizeMode ?? 'tail'}
              isHighlighted={isHighlighted}
              selectionColor={selectionColor}
              style={((rest: any): TextStyleProp)}
              ref={forwardedRef}
            />
          </TextAncestor.Provider>
        </View>
      );
    } else {
      return (
        <TextAncestor.Provider value={true}>
          <NativeText
            {...restProps}
            {...eventHandlersForText}
            accessible={accessible !== false}
            allowFontScaling={allowFontScaling !== false}
            ellipsizeMode={ellipsizeMode ?? 'tail'}
            isHighlighted={isHighlighted}
            selectionColor={selectionColor}
            style={style}
            ref={forwardedRef}
          />
        </TextAncestor.Provider>
      );
    }
  }
});

Text.displayName = 'Text';

// TODO: Delete this.
Text.propTypes = DeprecatedTextPropTypes;

/**
 * Returns false until the first time `newValue` is true, after which this will
 * always return true. This is necessary to lazily initialize `Pressability` so
 * we do not eagerly create one for every pressable `Text` component.
 */
function useLazyInitialization(newValue: boolean): boolean {
  const [oldValue, setValue] = useState(newValue);
  if (!oldValue && newValue) {
    setValue(newValue);
  }
  return oldValue;
}

// $FlowFixMe[incompatible-cast] - No good way to type a React.AbstractComponent with statics.
module.exports = (Text: typeof Text &
  $ReadOnly<{
    propTypes: typeof DeprecatedTextPropTypes,
  }>);
