
require "fileutils"

$BOARD = 'frdmk66f'
$root_dir = ""

def update_file(path, regular, replace)
    if File.exist?(path)
        File.open(path) do |fr|
            line = fr.read.gsub(regular, replace)
            File.open(path, "w") { |fw| fw.write(line) }
        end
        return true
    end
    return false
end

#update the USB stack version
def update_usb_stack_version()
    if ARGV.include?("-v")
        param = ARGV[ARGV.index("-v") + 1]
        if param.match(/[0-9]+\.[0-9]+\.[0-9]+/)
            regular = /([0-9]+)\.([0-9]+)\.([0-9]+)/
            regular.match(param)
            major = $1
            minor = $2
            bugfix = $3
            # puts "\r\nThe new version of USB stack is #{major}.#{minor}.#{bugfix}"
            # puts "#{$root_dir}/middleware/usb_#{param}/include/usb.h"
            if File.exist?("#{$root_dir}/middleware/usb_#{param}/include/usb.h")
                update_file("#{$root_dir}/middleware/usb_#{param}/include/usb.h",/USB_STACK_VERSION_MAJOR *\(.*\)/, "USB_STACK_VERSION_MAJOR \(#{major}U\)");
                update_file("#{$root_dir}/middleware/usb_#{param}/include/usb.h",/USB_STACK_VERSION_MINOR *\(.*\)/, "USB_STACK_VERSION_MINOR \(#{minor}U\)");
                update_file("#{$root_dir}/middleware/usb_#{param}/include/usb.h",/USB_STACK_VERSION_BUGFIX *\(.*\)/, "USB_STACK_VERSION_BUGFIX \(#{bugfix}U\)");
                # puts "The USB Stack version is updated!"
            else
                puts "Can't open the file #{$root_dir}/middleware/usb_#{param}/include/usb.h"
            end
            return
        end
    end
    puts("Invalid version parameter!")
end

def update_config_file_for_frdmk66f(path)
    if File.directory?(path)
        Dir.entries(path).each do |sub|
            if sub != '.' && sub != '..'
                if File.directory?("#{path}/#{sub}")
                    #puts "[#{sub}]"
                    update_config_file_for_frdmk66f("#{path}/#{sub}")
                else
                    # puts "  |--#{sub}"
                    if "#{sub}" == "usb_device_config.h"
                        # puts "Updating #{path}/#{sub}"
                        update_file("#{path}/#{sub}",/USB_DEVICE_CONFIG_KHCI *\(.*\)/, "USB_DEVICE_CONFIG_KHCI \(0U\)");
                        update_file("#{path}/#{sub}",/USB_DEVICE_CONFIG_EHCI *\(.*\)/, "USB_DEVICE_CONFIG_EHCI \(1U\)");
                    elsif "#{sub}" == "usb_host_config.h"
                        # puts "Updating #{path}/#{sub}"
                        update_file("#{path}/#{sub}",/USB_HOST_CONFIG_KHCI *\(.*\)/, "USB_HOST_CONFIG_KHCI \(0U\)");
                        update_file("#{path}/#{sub}",/USB_HOST_CONFIG_EHCI *\(.*\)/, "USB_HOST_CONFIG_EHCI \(1U\)");
                    end
                end
            end
        end
    end
end

def update_for_board()
    if ARGV.include?("-b")
        param = ARGV[ARGV.index("-b") + 1]
        if param.match(/frdmk66f/)
            $BOARD = 'frdmk66f'
            if File.directory?("#{$root_dir}") and File.directory?("#{$root_dir}/boards/#{$BOARD}/usb_examples")
                update_config_file_for_frdmk66f("#{$root_dir}/boards/#{$BOARD}/usb_examples")
                # puts "The config file for #{$BOARD} is updated!"
            else
                puts "Can't open the file #{$root_dir}/boards/#{$BOARD}/usb_examples"
            end
        end
        return
    end
    puts("Invalid board parameter!")
end

if ARGV.include?("-r")
    $root_dir = ARGV[ARGV.index("-r") + 1]
    # puts $root_dir
    if File.directory?("#{$root_dir}")
        update_usb_stack_version()
        update_for_board()
    else
        puts("Can't open the file #{$root_dir}")
    end
else
    puts("Invalid root dir parameter!")
end
