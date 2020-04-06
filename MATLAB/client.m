function client(port)
%   provides a menu for accessing PIC32 motor control functions
%
%   client(port)
%
%   Input Arguments:
%       port - the name of the com port.  This should be the same as what
%               you use in screen or putty in quotes ' '
%
%   Example:
%       client('/dev/ttyUSB0') (Linux/Mac)
%       client('COM3') (PC)
%
%   For convenience, you may want to change this so that the port is hardcoded.
   
% Opening COM connection
if ~isempty(instrfind)
    fclose(instrfind);
    delete(instrfind);
end

fprintf('Opening port %s....\n',port);

% settings for opening the serial port. baud rate 230400, hardware flow control
% wait up to 120 seconds for data before timing out
mySerial = serial(port, 'BaudRate', 230400, 'FlowControl', 'hardware','Timeout',120); 
% opens serial connection
fopen(mySerial);
% closes serial port when function exits
clean = onCleanup(@()fclose(mySerial));                                 

has_quit = false;
% menu loop
while ~has_quit
    fprintf('PIC32 MOTOR DRIVER INTERFACE\n\n');
    % display the menu options; this list will grow
    fprintf(['a: Read current sensor (ADCcounts)     b: Read current sensor (mA)\n'...
        'c: Read encoder (counts)               d: Read encoder (deg)\n'...
        'e: Reset encoder                       f: Set PWM (-100 to 100)\n'...
        'g: Set current gains                   h: Get current gains\n'...
        'i: Set position gains                  j: Get position gains\n'...
        'k: Test current control                l: Go to angle (deg)\n'...
        'm: Load step trajectory                n: Load cubic trajectory\n'...
        'o: Execute Trajectory                  p: Unpower the motor\n'...
        'q: Quit                                r: Get mode\n'...
        'z: Dummy Command                       x: Add\n']);
    
    % read the user's choice
    selection = input('\nENTER COMMAND: ', 's');
     
    % send the command to the PIC32
    fprintf(mySerial,'%c\n',selection);
    
    % take the appropriate action
    switch selection
        case 'a'
            %!!! MATLAB is freezing on fscanf
            n = fscanf(mySerial, '%d');
            fprintf('The motor current is %d ADC counts.\n',n);
        case 'b'
            n = fscanf(mySerial, '%d');
            fprintf('The motor current is %d mAn',n);
        case 'c'
            n = fscanf(mySerial, '%d');
            fprintf('The motor angle is %d counts.\n',n);
        case 'd'
            n = fscanf(mySerial, '%f');
            fprintf('The motor angle is %f degrees.\n',n); %!!!may want to change data type/decimal points
        case 'e'
            %computation will be handled by .c. May change to return motor angle in degrees
        case 'f'
            n = input('What PWM percentage would you like [-100 100]? ');
            fprintf(mySerial,'%d\n',n);
            if n < 0 && n >= -100
                fprintf('PWM has been set to %d%% in the clockwise direction.\n',n);
            elseif n >= 0 && n <= 100
                %!!! currently doesn't display properly
                fprintf('PWM has been set to %d%% in the counterclockwise direction.\n',n);
            else
                fprintf('Entered PWM percentage is out of range, try again.\n');
            end
            %!!!how to handle PWM = 0?
        case 'g'
            n = input('Enter your desired Kp current gain [recommended: 4.76]: ');
            m = input('Enter your desired Ki current gain [recommended: 0.32]: ');
            %chr = char([n,' ',m,'\n']);
            str = string({n,m,});
            str = join(str);
            fprintf('Sending Kp = %f and Ki = %f to the current controller.',n,m);
            fprintf(mySerial,'%s\n',str(1));
        case 'h'
            n = fscanf(mySerial,'%s');
            fprintf('The current controller is using Kp = %f and Ki = %f.', n(1), n(2));
        case 'i'
            n = input('Enter your desired Kp current gain [recommended: 4.76]: ');
            m = input('Enter your desired Ki current gain [recommended: 0.32]: ');
            o = input('Enter your desired Kd current gain [recommended: 10.63]: ');
            str = string({n,m,o});
            str = join(str);
            fprintf('Sending Kp = %f, Ki = %f, and Kd = %f to the current controller.',n,m,o);
            fprintf(mySerial,'%s\n',str);
        case 'j'
            n = fscanf(mySerial,'%s');
            fprintf('The current controller is using Kp = %f, Ki = %f, and Kd = %f.\n', n(1), n(2)), n(3);
        case 'k'
            %get motor current data from PIC32 then plot it
        case 'l'
            n = input('Enter the desired motor angle in degrees: ');
            fprintf(mySerial,'%d',n);
            fprintf('Motor moving to %d degrees.\n',n);
        case 'm'
            n = input('Enter step trajectory as an nx2 array, in seconds and degrees [time1, ang1; time2, ang2; ...]: ');
            t = 0;
            valid = true;
            for i = 1:size(n,1)
                if n(i,1) < t || n(i,1) > 10
                    valid = false;
                end
                t = n(i,1);
            end
            if valid
                fprintf('Plotting the desired trajectory and sending to the PIC32... ');
                ref = genRef(n,'step');
                fprintf(mySerial,'%d',ref);%!!!probably not %d format %!!!Also need to know max buffer size for PIC32
                n = scanf(mySerial,'%d');
                if n == 1
                    fprintf('completed.\n');
                else
                    fprintf('Error during transmission.\n');
                end
            else
                fprintf('Error: Maximum trajectory time is 10 seconds.\n');
            end
        case 'n'
            n = input('Enter step trajectory as an nx2 array, in seconds and degrees [time1, ang1; time2, ang2; ...]: ');
            t = 0;
            valid = true;
            for i = 1:size(n,1)
                if n(i,1) < t || n(i,1) > 10
                    valid = false;
                end
                t = n(i,1);
            end
            if valid
                fprintf('Plotting the desired trajectory and sending to the PIC32... ');
                ref = genRef(n,'cubic');
                fprintf(mySerial,'%d',ref);%!!!probably not %d format %!!!Also need to know max buffer size for PIC32
                n = scanf(mySerial,'%d');
                if n == 1
                    fprintf('completed.\n');
                else
                    fprintf('Error during transmission.\n');
                end
            else
                fprintf('Error: Maximum trajectory time is 10 seconds.\n');
            end
        case 'o'
            %do nothing. PIC will do computations. should probably remove
            %case statement
        case 'p'
            %do nothing. PIC will do computations
        case 'q'
            has_quit = true;             % exit client
        case 'r'
        case 'z'                         % example operation
            n = input('Enter number: '); % get the number to send
            fprintf(mySerial, '%d\n',n); % send the number
            n = fscanf(mySerial,'%d');   % get the incremented number back
            fprintf('Read: %d\n',n);     % print it to the screen
        case 'x'                         % example operation
            p = input('Enter two numbers (separated by a space): ', 's'); % get the number to send
            fprintf(mySerial, '%s\n',p); % send the number
            p = fscanf(mySerial,'%d');   % get the incremented number back
            fprintf('Read: %d\n',p);     % print it to the screen
        otherwise
            fprintf('Invalid Selection %c\n', selection);
    end
end

end
