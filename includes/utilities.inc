<?php

/**
 * Adds a token to a source url for rtmp streaming.
 * @param array $source
 * @return string
 */
function islandora_streaming_wrench_tokenize_rtmp($source, $token) {
  $url = parse_url($source['url']);
  $path = $url['path'];
  $path_parts = pathinfo($path);
  $filename = $path_parts['basename'];
  $application = $path_parts['dirname'];
  $source['url'] = $url['scheme'] . '://' . $url['host'] . ':' . $url['port'] .
    $application . "?t=$token/$filename";
  return $source;
}

/**
 * Adds a token to a source url for use with wrench.
 * @param array $source
 * @return string
 */
function islandora_streaming_wrench_tokenize($source, $token) {
  if (!empty($source['mime']) && $source['mime'] == 'rtmp/mp4') {
    $source = islandora_streaming_wrench_tokenize_rtmp($source, $token);
  }
  else {
    $source['url'] = $source['url'] . "?t=$token";
  }
  return $source;
}

/**
 * Creates a token for use with Wowza's Wrench plugin
 * @global object $user
 *   A drupal user object
 * @return mixed
 *   Returns a token on success, NULL on failure.
 */
function islandora_streaming_wrench_create_token() {
  $token = uniqid();
  $ip = ip_address();
  global $user;
  if (empty($user->name)) {
    $user->name = 'anonymous user';
  }
  $db_entry = array(
    'ip' => $ip,
    'token' => $token,
    'username' => $user->name,
    'ts' => date("Y-m-d H:i:s"),
  );
  try {
    $success = drupal_write_record('islandora_streaming_tokens', $db_entry);
  }
  catch (Exception $e) {
    drupal_set_message(t("Error creating secure token for video streaming. @details", array('@details' => $e->getMessage())), 'error');
  }
  return isset($success) ? $token : NULL;
}

/**
 * Remove unused Wrench Tokens.
 */
function islandora_streaming_wrench_remove_tokens() {
  try {
    $results = db_delete('islandora_streaming_tokens')
      ->where('ts <= DATE_SUB(CURDATE(), INTERVAL 1 HOUR)')
      ->execute();
  }
  catch (Exception $e) {
    watchdog('islandora_streaming_wrench', 'Error removing unused tokens. %msg', array('%msg' => $e->getMessage()), WATCHDOG_ERROR);
  }
}
