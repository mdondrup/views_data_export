<?php


interface ViewsDataExportExporterUserConfigurationInterface extends ViewsDataExportExporterInterface {

  function options_form(&$form, &$form_state);

  function options_submit(&$form, &$form_state);

}
